#pragma once

#include <models/community_model.h>
#include <utils/common.h>

#include <format>
#include <pqxx/pqxx>
#include <string>

#include "crow.h"
#include "models/user_model.h"
#include "utils/utils.h"

class AuthController {
	public:
	static void register_user(pqxx::connection& db, const crow::request& req, crow::response& res);
};
