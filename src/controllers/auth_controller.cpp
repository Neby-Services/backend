#include "controllers/auth_controller.h"

#include <utils/auth.h>

#include <string>

#include "bcrypt/BCrypt.hpp"
#include "utils/user_validations.h"

void AuthController::register_user(pqxx::connection &db, const crow::request &req, crow::response &res) {
	try {
		if (!is_correct_body(req, res)) return;

		crow::json::rvalue body = crow::json::load(req.body);

		std::string password = body["password"].s();
		std::string username = body["username"].s();
		std::string email = body["email"].s();
		std::string type = body["type"].s();

		std::string hash = BCrypt::generateHash(password);

		if (UserModel::user_exist(db, email, username)) {
			handle_error(res, "user alredy exist", 400);
			return;
		}

		UserModel user = UserModel::create_user(db, hash, email, username, "", 0, type);

		std::string token = create_token(user.getId());

		crow::json::wvalue data({{"token", token}});
		res.code = 200;
		res.write(data.dump());
		res.end();
	} catch (const std::exception &e) {
		std::cerr << "Error in register_user: " << e.what() << std::endl;
		handle_error(res, "INTERNAL SERVER ERROR", 500);
	}
}
