#include <controllers/auth_controller.h>
#include <utils/auth.h>
#include <utils/user_validations.h>

#include <bcrypt/BCrypt.hpp>
#include <ctime>  // Include the ctime header for time functions
#include <iomanip>
#include <string>

void AuthController::register_user(pqxx::connection &db, const crow::request &req, crow::response &res) {
	try {
		if (!is_correct_body_register(req, res)) return;

		crow::json::rvalue body = crow::json::load(req.body);

		std::string username = body["username"].s();
		std::string email = body["email"].s();
		std::string password = body["password"].s();
		std::string type = body["type"].s();
		std::string community_id;

		std::string hash = BCrypt::generateHash(password);

		if (UserModel::exists_username(db, username)) {
			handle_error(res, "username already in use", 400);
			return;
		}

		if (UserModel::exists_email(db, email)) {
			handle_error(res, "email already in use", 400);
			return;
		}

		if (type == Roles::ADMIN) {
			std::unique_ptr<CommunityModel> community = CommunityModel::create_community(db, body["community_name"].s());
			if (!community) {
				handle_error(res, "internal server error", 500);
				return;
			}
			community_id = community.get()->get_id();
		} else if (type == Roles::NEIGHBOR) {
			std::unique_ptr<CommunityModel> community = CommunityModel::get_community_by_code(db, body["community_code"].s());
			if (!community) {
				handle_error(res, "community does not exists", 404);
				return;
			}
			community_id = community.get()->get_id();
		}

		std::unique_ptr<UserModel> user = UserModel::create_user(db, community_id, username, email, hash, type, 0);

		if (!user) {
			handle_error(res, "internal server error", 500);
			return;
		}

		std::string jwtToken = create_token(user.get()->get_id(), type);
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

		crow::json::wvalue data({
			{"id", user.get()->get_id()},
		});

		res.code = 201;
		res.write(data.dump());
		res.end();
	} catch (const std::exception &e) {
		std::cerr << "Error in register_user: " << e.what() << std::endl;
		handle_error(res, "internal server error", 500);
	}
}

void AuthController::login_user(pqxx::connection &db, const crow::request &req, crow::response &res) {
	try {
		if (!is_correct_body_login(req, res)) return;

		crow::json::rvalue body = crow::json::load(req.body);

		std::string email = body["email"].s();

		std::unique_ptr<UserModel> user = UserModel::get_user_by_email(db, email);

		if (!user) {
			handle_error(res, "no user found with this email", 404);
			return;
		}

		const std::string encrypt_password = UserModel::get_password_by_email(db, email);

		std::string password = body["password"].s();

		if (BCrypt::validatePassword(password, encrypt_password)) {
			std::string jwtToken = create_token(user.get()->get_id(), user.get()->get_type());

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

			crow::json::wvalue data(
				{
					{"id", user.get()->get_id()},
				});

			res.code = 200;
			res.write(data.dump());

			res.end();
		} else {
			handle_error(res, "invalid password", 400);
			return;
		}

	} catch (const std::exception &e) {
		std::cerr << "Error in register_user: " << e.what() << std::endl;
		handle_error(res, "INTERNAL SERVER ERROR", 500);
	}
}
void get_self(pqxx::connection &db, const crow::request &req, crow::response &res) {
	try {
		crow::json::rvalue body = crow::json::load(req.body);
		std::string user_id = body["id"].s();
		std::unique_ptr<UserModel> user = UserModel::get_user_by_id(db, user_id);
		if (!user) {
			handle_error(res, "user not found", 404);
			return;
		}
		crow::json::wvalue user_data;
		user_data["id"] = user.get()->get_id();
		user_data["community_id"] = user.get()->get_community_id();
		user_data["username"] = user.get()->get_username();
		user_data["email"] = user.get()->get_email();
		user_data["type"] = user.get()->get_type();
		user_data["balance"] = user.get()->get_balance();
		user_data["created_at"] = user.get()->get_created_at();
		user_data["updated_at"] = user.get()->get_updated_at();

		crow::json::wvalue data{{"user", user_data}};
		res.code = 200;
		res.write(data.dump());
		res.end();

	} catch (const std::exception &e) {
		std::cerr << "Error in get self: " << e.what() << std::endl;
		handle_error(res, "internal server error", 500);
	}
}
