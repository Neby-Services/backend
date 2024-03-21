#include "controllers/auth_controller.h"

#include <utils/auth.h>

#include <ctime>  // Include the ctime header for time functions
#include <iomanip>
#include <string>

#include "bcrypt/BCrypt.hpp"
#include "utils/user_validations.h"

void AuthController::register_user(pqxx::connection &db, const crow::request &req, crow::response &res) {
	try {
		if (!is_correct_body_register(req, res)) return;

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

		// UserModel user = UserModel::create_user(db, hash, email, username, "", 0, type);

		UserModel user;

		if (type == Roles::ADMIN) {
			// ! check if community exist with name
			std::string community_name = body["community_name"].s();
			CommunityModel community = CommunityModel::create_community(db, community_name);
			user = UserModel::create_user(db, hash, email, username, "", 0, type);
			UserModel::set_community_id(db, community.get_id(), user.getId());

		} else if (type == Roles::NEIGHBOR) {
			// ! check if community exist with code
			std::string community_code = body["community_code"].s();
			// ! TODO:

			std::string community_id = CommunityModel::get_community_id(db, community_code);
			if (community_id == "") {
				handle_error(res, "not community exist", 404);
				return;
			}
			user = UserModel::create_user(db, hash, email, username, "", 0, type);
			UserModel::set_community_id(db, community_id, user.getId());
		}

		std::string jwtToken = create_token(user.getId(), type);
		int expirationTimeSeconds = 3600;
		time_t now = time(0);
		time_t expirationTime = now + expirationTimeSeconds;

		tm *expiration_tm = gmtime(&expirationTime);
		char expirationTimeStr[128];
		strftime(expirationTimeStr, 128, "%a, %d %b %Y %H:%M:%S GMT", expiration_tm);

		std::ostringstream cookieStream;
		cookieStream << "token=" << jwtToken << "; ";
		cookieStream << "Max-Age=" << expirationTimeSeconds << "; ";
		cookieStream << "Expires=" << expirationTimeStr << "; ";
		cookieStream << "Path=/api; ";
		cookieStream << "Secure";

		res.set_header("Set-Cookie", cookieStream.str());

		crow::json::wvalue data;
		data["user"] = {
			{"id", user.getId()},
		};

		res.code = 201;
		res.write(data.dump());
		res.end();
	} catch (const std::exception &e) {
		std::cerr << "Error in register_user: " << e.what() << std::endl;
		handle_error(res, "INTERNAL SERVER ERROR", 500);
	}
}

void AuthController::login_user(pqxx::connection &db, const crow::request &req, crow::response &res) {
	try {
		/* 	crow::json::rvalue body = crow::json::load(req.body);

			std::string id = body["id"].s();

			crow::json::wvalue data({{"id", id}});

			res.code = 200;
			res.write(data.dump());

			res.end(); */
		if (!is_correct_body_login(req, res)) return;

		crow::json::rvalue body = crow::json::load(req.body);

		std::string email = body["email"].s();

		std::unique_ptr<UserModel> user = UserModel::get_user_by_email(db, email);

		if (!user) {
			handle_error(res, "user by email not exist", 404);
			return;
		}

		const std::string encrypt_password = UserModel::get_password_by_email(db, email);

		std::string password = body["password"].s();

		if (BCrypt::validatePassword(password, encrypt_password)) {
			std::string jwtToken = create_token(user.get()->getId(), user.get()->getType());

			int expirationTimeSeconds = 3600;
			time_t now = time(0);
			time_t expirationTime = now + expirationTimeSeconds;

			tm *expiration_tm = gmtime(&expirationTime);
			char expirationTimeStr[128];
			strftime(expirationTimeStr, 128, "%a, %d %b %Y %H:%M:%S GMT", expiration_tm);

			std::ostringstream cookieStream;
			cookieStream << "token=" << jwtToken << "; ";
			cookieStream << "Max-Age=" << expirationTimeSeconds << "; ";
			cookieStream << "Expires=" << expirationTimeStr << "; ";
			cookieStream << "Path=/api; ";
			cookieStream << "Secure";

			res.set_header("Set-Cookie", cookieStream.str());

			crow::json::wvalue data;
			data["user"] = {
				{"id", user.get()->getId()},
				{"type", user.get()->getType()}};

			res.code = 200;
			res.write(data.dump());

			res.end();
		} else {
			handle_error(res, "password invalid", 401);
			return;
		}

	} catch (const std::exception &e) {
		std::cerr << "Error in register_user: " << e.what() << std::endl;
		handle_error(res, "INTERNAL SERVER ERROR", 500);
	}
}
