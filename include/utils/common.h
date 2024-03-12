// common.h
#pragma once
#include <crow.h>

#include <map>
#include <string>

#include "bcrypt/BCrypt.hpp"

// ** middleawares includes
#include "middlewares/user_validation_middleware.h"

using NebyApp = crow::App<SignupValidation>;

struct Roles {
	static const std::string ADMIN;
	static const std::string NEIGHBOR;
};
