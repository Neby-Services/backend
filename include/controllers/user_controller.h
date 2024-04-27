#pragma once

#include <crow.h>
#include <models/user_model.h>
#include <utils/errors.h>
#include <utils/user_validations.h>
#include <utils/utils.h>
#include <format>
#include <pqxx/pqxx>
#include <string>
#include <vector>

class UserController {
	public:
	static void get_users(pqxx::connection& db, const crow::request& req, crow::response& res);
	static void get_user_by_id(pqxx::connection& db, const crow::request& req, crow::response& res, const std::string& user_id);
	static void delete_user_by_id(pqxx::connection& db, crow::response& res, const std::string& user_id);
	static void update_user_by_id(pqxx::connection& db, const crow::request& req, crow::response& res, const std::string& user_id);
	static void update_self(pqxx::connection& db, const crow::request& req, crow::response& res);
};
