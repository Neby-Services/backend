#pragma once

#include <map>
#include <string>

#include "bcrypt/BCrypt.hpp"
#include "crow.h"
#include "crow/middlewares/cookie_parser.h"
#include "middlewares/user_validation_middleware.h"

using NebyApp = crow::App<SignupValidation, crow::CookieParser>;

struct Roles {
	static const std::string ADMIN;
	static const std::string NEIGHBOR;
};
