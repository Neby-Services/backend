#pragma once

#include <format>
#include <pqxx/pqxx>
#include <string>
#include <vector>

#include "crow.h"
#include "models/user_model.h"

class UserController {
	public:
	static void get_users(pqxx::connection& db, const crow::request& req, crow::response& res);
};
