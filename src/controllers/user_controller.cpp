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

