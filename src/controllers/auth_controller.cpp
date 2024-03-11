#include "controllers/auth_controller.h"

void AuthController::register_user(pqxx::connection& db, const crow::request& req, crow::response& res) {
	crow::json::rvalue body = crow::json::load(req.body);

	std::string password = body["password"].s();
	std::string username = body["username"].s();
	std::string email = body["email"].s();
	std::string type = body["type"].s();

	UserModel user = UserModel::create_user(db, password, email, username, "", 0, type);

	crow::json::wvalue data({{"id", user.getId()}});
	res.code = 200;
	res.write(data.dump());
	res.end();
}
