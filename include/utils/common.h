#pragma once

#include <map>
#include <string>

#include "bcrypt/BCrypt.hpp"
#include "crow.h"
#include "middlewares/user_validation_middleware.h"

using NebyApp = crow::App<SignupValidation>;

struct Roles {
	static const std::string ADMIN;
	static const std::string NEIGHBOR;
};
