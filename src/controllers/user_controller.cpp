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
