#include "controllers/user_controller.h"

void UserController::get_users(pqxx::connection &db, const crow::request &req, crow::response &res) {
	try {
		std::vector<UserModel> allUsers = UserModel::get_users(db);
		crow::json::wvalue::list users;
		for (unsigned int i = 0; i < allUsers.size(); i++) {
			crow::json::wvalue user;
			user["id"] = allUsers[i].getId();
			user["email"] = allUsers[i].getEmail();
			user["username"] = allUsers[i].getUsername();
			user["image_url"] = allUsers[i].getImageUrl();
			user["balance"] = allUsers[i].getBalance();
			user["type"] = allUsers[i].getType();
			users.push_back(user);
		}
		crow::json::wvalue data{{"users", users}};
		res.code = 200;
		res.write(data.dump());
		res.end();
	} catch (const std::exception &e) {
		std::cerr << "Error in get users: " << e.what() << std::endl;
		res.code = 500;
		crow::json::wvalue error({{"error", "internal server error"}});
		res.write(error.dump());
		res.end();
	}
}

void UserController::get_user_by_id(pqxx::connection &db, const crow::request &req, const std::string &user_id, crow::response &res) {
	try {
		if (user_id.empty()) {
			res.code = 400;
			crow::json::wvalue error({{"error", "id doesn't exist"}});
			res.write(error.dump());
			res.end();
			return;
		}

		UserModel user = UserModel::get_user_by_id(db, user_id);

		crow::json::wvalue user_data;
		user_data["id"] = user.getId();
		user_data["email"] = user.getEmail();
		user_data["username"] = user.getUsername();
		user_data["image_url"] = user.getImageUrl();
		user_data["balance"] = user.getBalance();
		user_data["type"] = user.getType();

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

void UserController::delete_user_by_id(pqxx::connection &db, const std::string &user_id, crow::response &res) {
	try {
		bool elimin = UserModel::delete_by_id(db, user_id);

		if (elimin) {
			res.code = 200;
			crow::json::wvalue response_message;
			response_message["message"] = "User deleted successfully";
			res.write(response_message.dump());
		} else {
			res.code = 404;
			crow::json::wvalue error_message;
			error_message["error"] = "User not found";
			res.write(error_message.dump());
		}
		res.end();
	} catch (const std::exception &e) {
		std::cerr << "Error deleting user: " << e.what() << std::endl;
		res.code = 500;
		res.end();
	}
}

void UserController::update_user_by_id(pqxx::connection &db, const std::string &user_id, const crow::request &req, crow::response &res) {
	try {
		bool userFound = UserModel::user_exist_by_id(db, user_id);
		if (userFound) {
			crow::json::rvalue update = crow::json::load(req.body);
			std::string temp_name = "", temp_pass = "", temp_email = "";
			if (update.has("username")) {
				temp_name = update["username"].s();
				if (!validate_username(temp_name, res)) {
					res.code = 400;
					crow::json::wvalue error_message;
					error_message["error"] = "incorrect username";
					res.write(error_message.dump());
					return;
				}
			}
			if (update.has("email")) {
				temp_email = update["email"].s();
				if (!validate_email(temp_email, res)) {
					res.code = 400;
					crow::json::wvalue error_message;
					error_message["error"] = "incorrect email";
					res.write(error_message.dump());
					return;
				}
			}
			if (update.has("password")) {
				temp_pass = update["password"].s();
				if (!validate_password(temp_pass, res)) {
					res.code = 400;
					crow::json::wvalue error_message;
					error_message["error"] = "incorrect password";
					res.write(error_message.dump());
					return;
				}
			}
			std::string hash = BCrypt::generateHash(temp_pass);
			bool succes = UserModel::update_by_id(db, user_id, temp_name, temp_email, hash);
			if (succes) {
				res.code = 200;
				crow::json::wvalue response_message;
				response_message["message"] = "User updated successfully";
				res.write(response_message.dump());
			} else {
				res.code = 400;
				crow::json::wvalue error_message;
				error_message["error"] = "Failed to update user";
				res.write(error_message.dump());
			}
			res.end();
		} else {
			res.code = 404;
			crow::json::wvalue error_message;
			error_message["error"] = "User not found";
			res.write(error_message.dump());
		}
	} catch (const std::exception &e) {
		std::cerr << "Error updating user: " << e.what() << std::endl;
		res.code = 500;
		res.end();
	}
}
