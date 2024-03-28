#include <controllers/user_controller.h>

void UserController::get_users(pqxx::connection &db, const crow::request &req, crow::response &res) {
	try {
		std::vector<std::unique_ptr<UserModel>> allUsers = UserModel::get_users(db);
		crow::json::wvalue::list users;
		for (unsigned int i = 0; i < allUsers.size(); ++i) {
			crow::json::wvalue user;
			user["id"] = allUsers[i].get()->getId();
			user["community_id"] = allUsers[i].get()->getCommunityId();
			user["username"] = allUsers[i].get()->getUsername();
			user["email"] = allUsers[i].get()->getEmail();
			user["type"] = allUsers[i].get()->getType();
			user["balance"] = allUsers[i].get()->getBalance();
			user["created_at"] = allUsers[i].get()->getCreatedAt();
			user["updated_at"] = allUsers[i].get()->getUpdatedAt();
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
		user_data["id"] = user.get()->getId();
		user_data["community_id"] = user.get()->getCommunityId();
		user_data["username"] = user.get()->getUsername();
		user_data["email"] = user.get()->getEmail();
		user_data["type"] = user.get()->getType();
		user_data["balance"] = user.get()->getBalance();
		user_data["created_at"] = user.get()->getCreatedAt();
		user_data["updated_at"] = user.get()->getUpdatedAt();

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

void UserController::delete_user_by_id(pqxx::connection &db, crow::response &res, const std::string &user_id) {
	try {
		bool deleted = UserModel::delete_user_by_id(db, user_id);

		if (deleted) {
			res.code = 200;
			crow::json::wvalue response_message;
			response_message["message"] = "user deleted successfully";
			res.write(response_message.dump());
			res.end();
		} else
			handle_error(res, "user not found", 404);
	} catch (const std::exception &e) {
		std::cerr << "Error deleting user: " << e.what() << std::endl;
		handle_error(res, "internal server error", 500);
	}
}

void UserController::update_user_by_id(pqxx::connection &db, const crow::request &req, crow::response &res, const std::string &user_id) {
	try {
		bool userFound = UserModel::exists_id(db, user_id);
		if (userFound) {
			crow::json::rvalue update = crow::json::load(req.body);
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
		} else
			handle_error(res, "user not found", 404);
	} catch (const std::exception &e) {
		std::cerr << "Error updating user: " << e.what() << std::endl;
		handle_error(res, "internal server error", 500);
	}
}
