#pragma once

#include <crow.h>
#include <models/service_model.h>
#include <utils/errors.h>
#include <utils/utils.h>

#include <format>
#include <memory>
#include <pqxx/pqxx>
#include <string>
#include <vector>

class ServiceController {
	public:
	/* static void get_users(pqxx::connection& db, const crow::request& req, crow::response& res);
	static void get_user_by_id(pqxx::connection& db, const crow::request& req, const std::string& user_id, crow::response& res);
	static void delete_user_by_id(pqxx::connection& db, const std::string& user_id, crow::response& res); */

	static void create_service(pqxx::connection &db, const crow::request &req, crow::response &res);
};