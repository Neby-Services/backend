#pragma once

#include <crow.h>
#include <jwt-cpp/jwt.h>
#include <utils/auth.h>
#include <utils/utils.h>

struct VerifyJWT : crow::ILocalMiddleware {
	struct context {};

	void before_handle(crow::request& req, crow::response& res, context& ctx);
	void after_handle(crow::request& req, crow::response& res, context& ctx);
};
