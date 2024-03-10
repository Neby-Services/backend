#ifndef USER_CONTROLLER_H
#define USER_CONTROLLER_H

#include <format>
#include <pqxx/pqxx>
#include <string>

#include "crow.h"

class UserController {
	public:
	static std::string index(pqxx::connection& db);
	static void create_user(pqxx::connection& db, const crow::request& req, crow::response& res);
};

#endif