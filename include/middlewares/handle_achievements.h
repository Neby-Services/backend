
#pragma once

#include <crow.h>
#include <db/connection_pool.h>
#include <jwt-cpp/jwt.h>
#include <utils/auth.h>
#include <utils/common.h>
#include <utils/utils.h>
#include <memory>
#include <utils/user_achievements_handler.h>

struct HandleAchievements : crow::ILocalMiddleware {
	struct context {};

	void before_handle(crow::request& req, crow::response& res, context& ctx);
	void after_handle(crow::request& req, crow::response& res, context& ctx);
};
