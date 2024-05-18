#pragma once

#include <crow.h>
#include <models/community_model.h>
#include <models/user_model.h>
#include <models/user_achievement_model.h>
#include <utils/auth.h>
#include <utils/common.h>
#include <utils/user_validations.h>
#include <utils/utils.h>
#include <bcrypt/BCrypt.hpp>
#include <ctime>  // Include the ctime header for time functions
#include <format>
#include <iomanip>
#include <memory>
#include <pqxx/pqxx>
#include <string>
 
class AuthController {
	public:
	static void register_user(pqxx::connection& db, const crow::request& req, crow::response& res);
	static void login_user(pqxx::connection& db, const crow::request& req, crow::response& res);
	static void get_self(pqxx::connection& db, const crow::request& req, crow::response& res);
};
