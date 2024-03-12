#include "controllers/auth_controller.h"

#include <string>

#include "bcrypt/BCrypt.hpp"

void AuthController::register_user(pqxx::connection &db, const crow::request &req, crow::response &res)
{
	try
	{
		crow::json::rvalue body = crow::json::load(req.body);

		std::string password = body["password"].s();
		std::string username = body["username"].s();
		std::string email = body["email"].s();
		std::string type = body["type"].s();

		std::string hash = BCrypt::generateHash(password);

		if (UserModel::user_exist(db, email))
		{
			res.code = 400;
			crow::json::wvalue error({{"error", "user already exists"}});
			res.write(error.dump());
			res.end();
			return;
		}

		if (UserModel::username_exist(db, username))
		{
			res.code = 400;
			crow::json::wvalue error({{"error", "username already exists"}});
			res.write(error.dump());
			res.end();
			return;
		}

		UserModel user = UserModel::create_user(db, hash, email, username, "", 0, type);

		crow::json::wvalue data({{"id", user.getId()}});
		res.code = 200;
		res.write(data.dump());
		res.end();
	}
	catch (const std::exception &e)
	{
		std::cerr << "Error in register_user: " << e.what() << std::endl;
		res.code = 500;
		crow::json::wvalue error({{"error", "internal server error"}});
		res.write(error.dump());
		res.end();
	}
}
