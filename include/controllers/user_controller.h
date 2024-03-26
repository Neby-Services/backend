#pragma once

#include <utils/errors.h>
#include <utils/utils.h>

#include <format>
#include <pqxx/pqxx>
#include <string>
#include <vector>

#include "crow.h"
#include "models/user_model.h"
#include "utils/user_validations.h"

class UserController {
	public:
	static void get_users(pqxx::connection& db, const crow::request& req, crow::response& res);
	static void get_user_by_id(pqxx::connection& db, const crow::request& req, const std::string& user_id, crow::response& res);
	static void delete_user_by_id(pqxx::connection& db, const std::string& user_id, crow::response& res);
	static void update_user_by_id(pqxx::connection& db, const std::string& user_id, const crow::request& req, crow::response& res);
};
