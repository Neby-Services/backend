#include "controllers/auth_controller.h"

#include <string>

#include "bcrypt/BCrypt.hpp"

void AuthController::register_user(pqxx::connection& db, const crow::request& req, crow::response& res) {
	crow::json::rvalue body = crow::json::load(req.body);

	std::string password = body["password"].s();
	std::string username = body["username"].s();
	std::string email = body["email"].s();
	std::string type = body["type"].s();

	std::string hash = BCrypt::generateHash(password);

	UserModel user = UserModel::create_user(db, hash, email, username, "", 0, type);

	crow::json::wvalue data({{"id", user.getId()}});
	res.code = 200;
	res.write(data.dump());
	res.end();
}
