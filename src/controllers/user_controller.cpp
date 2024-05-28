#include <controllers/user_controller.h>

void UserController::get_users(pqxx::connection& db, const crow::request& req, crow::response& res) {
	try {

		crow::json::rvalue body = crow::json::load(req.body);

		const std::string admin_community_id = body["request_community_id"].s();

		std::vector<UserModel> allUsers = UserModel::get_users_admin(db, admin_community_id);

		crow::json::wvalue::list users;
		for (unsigned int i = 0; i < allUsers.size(); ++i) {
			crow::json::wvalue user;
			user["id"] = allUsers[i].get_id();
			user["community_id"] = allUsers[i].get_community_id();
			user["username"] = allUsers[i].get_username();
			user["email"] = allUsers[i].get_email();
			user["type"] = allUsers[i].get_type();
			user["balance"] = allUsers[i].get_balance();
			user["created_at"] = allUsers[i].get_created_at();
			user["updated_at"] = allUsers[i].get_updated_at();
			users.push_back(user);
		}
		crow::json::wvalue data{ {"users", users} };
		res.code = 200;
		res.write(data.dump());
		res.end();
	}
	catch (const pqxx::data_exception& e) {
		CROW_LOG_ERROR << "PQXX execption: " << e.what();
		handle_error(res, "invalid id", 400);
	}
	catch (const std::exception& e) {
		CROW_LOG_ERROR << "Error in get_users controller: " << e.what();
		handle_error(res, "internal server error", 500);
	}
}

void UserController::get_user_by_id(pqxx::connection& db, const crow::request& req, crow::response& res, const std::string& user_id) {
	try {
		std::unique_ptr<UserModel> user = UserModel::get_user_by_id(db, user_id);

		if (!user) {
			handle_error(res, "user not found", 404);
			return;
		}

		crow::json::wvalue user_data;
		user_data["id"] = user.get()->get_id();
		user_data["username"] = user.get()->get_username();
		user_data["email"] = user.get()->get_email();
		user_data["type"] = user.get()->get_type();
		user_data["balance"] = user.get()->get_balance();
		user_data["created_at"] = user.get()->get_created_at();
		user_data["updated_at"] = user.get()->get_updated_at();
		user_data["community_id"] = user.get()->get_community_id();

		crow::json::wvalue community_data_json;
		community_data_json["id"] = user.get()->get_community().value().get_id();
		community_data_json["name"] = user.get()->get_community().value().get_name();
		community_data_json["code"] = user.get()->get_community().value().get_code();
		community_data_json["created_at"] = user.get()->get_community().value().get_created_at();
		community_data_json["updated_at"] = user.get()->get_community().value().get_updated_at();

		user_data["community"] = crow::json::wvalue(community_data_json);

		crow::json::wvalue data{ {"user", user_data} };
		res.code = 200;
		res.write(data.dump());
		res.end();

	}
	catch (const pqxx::data_exception& e) {
		CROW_LOG_ERROR << "PQXX execption: " << e.what();
		handle_error(res, "invalid id", 400);
	}
	catch (const data_not_found_exception& e) {
		CROW_LOG_ERROR << "Data not found exception: " << e.what();
		handle_error(res, e.what(), 404);
	}

	catch (const std::exception& e) {
		CROW_LOG_ERROR << "Error get_user_by_id controller: " << e.what();
		handle_error(res, e.what(), 500);
	}
}

void UserController::update_user_by_id(pqxx::connection& db, const crow::request& req, crow::response& res, const std::string& user_id) {
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

				UserModel::update_user_by_id(db, user_id, temp_name, "", "", temp_balance, true);
				res.code = 200;
				crow::json::wvalue response_message;
				response_message["message"] = "User updated successfully";
				res.write(response_message.dump());
				res.end();
			}
			else {
				handle_error(res, "does not belong to your community", 403);
			}
		}
		else
			handle_error(res, "not enough privileges", 403);
	}
	catch (const pqxx::data_exception& e) {
		CROW_LOG_ERROR << "PQXX execption: " << e.what();
		handle_error(res, "invalid id", 400);
	}
	catch (const update_exception& e) {
		CROW_LOG_ERROR << "Update exception: " << e.what();
		handle_error(res, e.what(), 404);
	}
	catch (const std::exception& e) {
		CROW_LOG_ERROR << "Error update_user controller: " << e.what();
		handle_error(res, "internal server error", 500);
	}
}

void UserController::update_self(pqxx::connection& db, const crow::request& req, crow::response& res) {
	try {
		crow::json::rvalue update = crow::json::load(req.body);
		std::string user_id = update["id"].s();
		std::string temp_name = "", temp_pass = "", temp_email = "";
		int temp_balance = -1;
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
		if (update.has("balance")) {
			temp_balance = update["balance"].i();
			if (temp_balance < 0) {
				handle_error(res, "balance must be > 0", 400);
				return;
			}
		}

		std::string hash = "";
		if (temp_pass != "")
			hash = BCrypt::generateHash(temp_pass);

		UserModel::update_user_by_id(db, user_id, temp_name, temp_email, hash, temp_balance, true);

		res.code = 200;
		crow::json::wvalue response_message;
		response_message["message"] = "User updated successfully";
		res.write(response_message.dump());
		res.end();

	}
	catch (const pqxx::data_exception& e) {
		CROW_LOG_ERROR << "PQXX execption: " << e.what();
		handle_error(res, "invalid id", 400);
	}
	catch (const update_exception& e) {
		CROW_LOG_ERROR << "Update exception: " << e.what();
		handle_error(res, e.what(), 404);
	}
	catch (const std::exception& e) {
		CROW_LOG_ERROR << "Error update_self controller: " << e.what();
		handle_error(res, "internal server error", 500);
	}
}

void UserController::delete_self(pqxx::connection& db, const crow::request& req, crow::response& res) {
	try {
		crow::json::rvalue request = crow::json::load(req.body);
		std::string user_id = request["id"].s();

		if (request["isAdmin"].b() == true) {
			handle_error(res, "admin can't delete themselves", 403);
		}

		UserModel::delete_user_by_id(db, user_id, true);

		res.code = 200;
		crow::json::wvalue response_message;
		response_message["message"] = "user deleted successfully";
		res.write(response_message.dump());
		res.end();
	}
	catch (const pqxx::data_exception& e) {
		CROW_LOG_ERROR << "PQXX execption: " << e.what();
		handle_error(res, "invalid id", 400);
	}
	catch (const deletion_exception& e) {
		CROW_LOG_ERROR << "Delete exception: " << e.what();
		handle_error(res, e.what(), 404);
	}
	catch (const std::exception& e) {
		CROW_LOG_ERROR << "Error delete_user controller: " << e.what();
		handle_error(res, "internal server error", 500);
	}
}

void UserController::delete_user_by_id(pqxx::connection& db, const crow::request& req, crow::response& res, const std::string& user_id) {
	try {
		crow::json::rvalue request = crow::json::load(req.body);
		if (request["isAdmin"].b() == false) {
			handle_error(res, "not enough privileges", 403);
			return;
		}

		else if (!isValidUUID(user_id)) {
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
			UserModel::delete_user_by_id(db, user_id, true);
			res.code = 200;
			crow::json::wvalue response_message;
			response_message["message"] = "user deleted successfully";
			res.write(response_message.dump());
			res.end();
		}
		else {
			handle_error(res, "not enough privileges", 403);
		}
	}
	catch (const pqxx::data_exception& e) {
		CROW_LOG_ERROR << "PQXX execption: " << e.what();
		handle_error(res, "invalid id", 400);
	}
	catch (const deletion_exception& e) {
		CROW_LOG_ERROR << "Delete exception: " << e.what();
		handle_error(res, e.what(), 404);
	}
	catch (const std::exception& e) {
		CROW_LOG_ERROR << "Error delete_user_by_id controller: " << e.what();
		handle_error(res, "internal server error", 500);
	}
}
