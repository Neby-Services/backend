#pragma once

#include <crow.h>

#include "utils/user_validations.h"

struct SignupValidation : crow::ILocalMiddleware {
	struct context {};

	void before_handle(crow::request& req, crow::response& res, context& ctx) {}

	void after_handle(crow::request& req, crow::response& res, context& ctx) {}
};
