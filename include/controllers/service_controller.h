#pragma once

#include <crow.h>

#include <format>
#include <memory>
#include <pqxx/pqxx>
#include <string>
#include <vector>

// ** custom includes
#include <models/service_model.h>
#include <models/user_model.h>
#include <utils/common.h>
#include <utils/errors.h>
#include <utils/utils.h>
#include <utils/validations.h>
// ** ---------------------------------------------

class ServiceController {
	public:
	/* static void get_users(pqxx::connection& db, const crow::request& req, crow::response& res);
	static void get_user_by_id(pqxx::connection& db, const crow::request& req, const std::string& user_id, crow::response& res);
	static void delete_user_by_id(pqxx::connection& db, const std::string& user_id, crow::response& res); */

	static void create_service(pqxx::connection& db, const crow::request& req, crow::response& res);
	static void get_services(pqxx::connection& db, const crow::request& req, crow::response& res);
	static void delete_service(pqxx::connection& db, const crow::request& req, crow::response& res, std::string service_id);
};
