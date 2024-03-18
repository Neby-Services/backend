#include "controllers/auth_controller.h"

#include <utils/auth.h>

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

		UserModel user = UserModel::create_user(db, hash, email, username, "", 0, type);

		if (type == Roles::ADMIN) {
			// ! check if community exist with name
			std::string community_name = body["community_name"].s();
			CommunityModel community = CommunityModel::create_community(db, community_name);
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
			UserModel::set_community_id(db, community_id, user.getId());
		}

		std::string token = create_token(user.getId(), type);

		std::string token_cookie = "token=" + token;

		res.add_header("Set-Cookie", token_cookie);

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
			std::string token = create_token(user.get()->getId(), user.get()->getType());

			std::string token_cookie = "token=" + token;

			res.add_header("Set-Cookie", token_cookie);

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
