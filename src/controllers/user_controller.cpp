#include <controllers/user_controller.h>

void UserController::get_users(pqxx::connection &db, const crow::request &req, crow::response &res) {
	try {
		std::vector<std::unique_ptr<UserModel>> allUsers = UserModel::get_users(db);
		crow::json::wvalue::list users;
		for (unsigned int i = 0; i < allUsers.size(); ++i) {
			crow::json::wvalue user;
			user["id"] = allUsers[i].get()->get_id();
			user["community_id"] = allUsers[i].get()->get_community_id();
			user["username"] = allUsers[i].get()->get_username();
			user["email"] = allUsers[i].get()->get_email();
			user["type"] = allUsers[i].get()->get_type();
			user["balance"] = allUsers[i].get()->get_balance();
			user["created_at"] = allUsers[i].get()->get_created_at();
			user["updated_at"] = allUsers[i].get()->get_updated_at();
			users.push_back(user);
		}
		crow::json::wvalue data{{"users", users}};
		res.code = 200;
		res.write(data.dump());
		res.end();
	} catch (const std::exception &e) {
		std::cerr << "Error in get users: " << e.what() << std::endl;
		handle_error(res, "internal server error", 500);
	}
}

void UserController::get_user_by_id(pqxx::connection &db, const crow::request &req, crow::response &res, const std::string &user_id) {
	try {
		if (user_id.empty()) {
			handle_error(res, "id must be provided", 400);
			return;
		}

		std::unique_ptr<UserModel> user = UserModel::get_user_by_id(db, user_id);

		if (!user) {
			handle_error(res, "user not found", 404);
			return;
		}

		crow::json::wvalue user_data;
		user_data["id"] = user.get()->get_id();
		user_data["community_id"] = user.get()->get_community_id();
		user_data["username"] = user.get()->get_username();
		user_data["email"] = user.get()->get_email();
		user_data["type"] = user.get()->get_type();
		user_data["balance"] = user.get()->get_balance();
		user_data["created_at"] = user.get()->get_created_at();
		user_data["updated_at"] = user.get()->get_updated_at();

		crow::json::wvalue data{{"user", user_data}};
		res.code = 200;
		res.write(data.dump());
		res.end();

	} catch (const pqxx::data_exception &e) {
		handle_error(res, "invalid id", 400);
	}

	catch (const data_not_found_exception &e) {
		handle_error(res, e.what(), 404);
	}

	catch (const std::exception &e) {
		handle_error(res, e.what(), 500);
	}
}

void UserController::delete_user_by_id(pqxx::connection &db, const crow::request &req, crow::response &res, const std::string &user_id)
{
	try
	{
		crow::json::rvalue request = crow::json::load(req.body);
		if (request["isAdmin"].b() == false)
		{
			handle_error(res, "not enough privileges", 403);
			return;
		}

		else if (!isValidUUID(user_id))
		{
			handle_error(res, "invalid id", 400);
			return;
		}

		std::unique_ptr<UserModel> user = UserModel::get_user_by_id(db, user_id);
		if (!user) {
				handle_error(res, "user not found", 404);
				return;
		}

		std::string user_community = user.get()->get_community_id();

		std::unique_ptr<UserModel> admin = UserModel::get_user_by_id(db, request["id"].s());
		std::string admin_community = admin.get()->get_community_id();
		
		if (user_community == admin_community) {
				bool deleted = UserModel::delete_user_by_id(db, user_id);
				if (deleted)
				{
					res.code = 200;
					crow::json::wvalue response_message;
					response_message["message"] = "user deleted successfully";
					res.write(response_message.dump());
					res.end();
				}
				else
					handle_error(res, "user not found", 404);
			}

		else {
				handle_error(res, "not enough privileges", 403);
			}
	}
	catch (const std::exception &e)
	{
		std::cerr << "Error deleting user: " << e.what() << std::endl;
		handle_error(res, "internal server error", 500);
	}
}

void UserController::update_user_by_id(pqxx::connection &db, const crow::request &req, crow::response &res, const std::string &user_id) {
	try {
		crow::json::rvalue update = crow::json::load(req.body);
		if (update["isAdmin"].b() == true) {
			if (user_id.empty()) {
				handle_error(res, "id must be provided", 400);
				return;
			}

			if (!isValidUUID(user_id)) {
				handle_error(res, "invalid id", 400);
				return;
			}

			std::unique_ptr<UserModel> user = UserModel::get_user_by_id(db, user_id);

			if (!user) {
				handle_error(res, "user not found", 404);
				return;
			}

			std::string user_community = user.get()->get_community_id();

			std::unique_ptr<UserModel> admin = UserModel::get_user_by_id(db, update["id"].s());
			std::string admin_community = admin.get()->get_community_id();

			if (user_community == admin_community) {
				crow::json::rvalue update = crow::json::load(req.body);
				std::string temp_name = "";
				int temp_balance = -1;
				if (update.has("username")) {
					temp_name = update["username"].s();
					// validate username currently throws an error, so this return and error messages ar not being used
					if (!validate_username(temp_name, res)) {
						handle_error(res, "incorrect username", 400);
						return;
					};
				}
				if (update.has("balance")) {
					temp_balance = update["balance"].i();
					if (temp_balance < 0) {
						handle_error(res, "invalid balance", 400);
						return;
					}
				}
				bool succes = UserModel::update_user_admin(db, user_id, temp_name, temp_balance);
				if (succes) {
					res.code = 200;
					crow::json::wvalue response_message;
					response_message["message"] = "User updated successfully";
					res.write(response_message.dump());
					res.end();
				} else
					handle_error(res, "internal server error", 500);
			} else {
				handle_error(res, "not enough privileges", 403);
			}
		} else
			handle_error(res, "not enough privileges", 403);
	} catch (const std::exception &e) {
		std::cerr << "Error updating user: " << e.what() << std::endl;
		handle_error(res, "internal server error", 500);
	}
}

void UserController::update_self(pqxx::connection &db, const crow::request &req, crow::response &res) {
	try {
		crow::json::rvalue update = crow::json::load(req.body);
		std::string user_id = update["id"].s();
		std::string temp_name = "", temp_pass = "", temp_email = "";
		if (update.has("username")) {
			temp_name = update["username"].s();
			if (!validate_username(temp_name, res)) return;
		}
		if (update.has("email")) {
			temp_email = update["email"].s();
			if (!validate_email(temp_email, res)) return;
		}
		if (update.has("password")) {
			temp_pass = update["password"].s();
			if (!validate_password(temp_pass, res)) return;
		}
		std::string hash = BCrypt::generateHash(temp_pass);
		bool succes = UserModel::update_user_by_id(db, user_id, temp_name, temp_email, hash);
		if (succes) {
			res.code = 200;
			crow::json::wvalue response_message;
			response_message["message"] = "User updated successfully";
			res.write(response_message.dump());
			res.end();
		} else
			handle_error(res, "internal server error", 500);
	} catch (const std::exception &e) {
		std::cerr << "Error updating user: " << e.what() << std::endl;
		handle_error(res, "internal server error", 500);
	}
}

void UserController::delete_self(pqxx::connection &db, const crow::request &req, crow::response &res)
{
	try
	{
		crow::json::rvalue request = crow::json::load(req.body);
		std::string user_id = request["id"].s();
		if (request["isAdmin"].b() == true)
		{
			handle_error(res, "admin can't delete themselves", 403);
		}

		bool deleted = UserModel::delete_user_by_id(db, user_id);

		if (deleted)
		{
			res.code = 200;
			crow::json::wvalue response_message;
			response_message["message"] = "user deleted successfully";
			res.write(response_message.dump());
			res.end();
		}
		else
			handle_error(res, "user not found", 404);
	}
	catch (const std::exception &e)
	{
		std::cerr << "Error deleting user: " << e.what() << std::endl;
		handle_error(res, "internal server error", 500);
	}
}