#pragma once
#include <crow.h>
#include <middlewares/verify_jwt.h>

#include <map>
#include <string>

#include "bcrypt/BCrypt.hpp"

using NebyApp = crow::App<VerifyJWT>;

struct Roles {
	static const std::string ADMIN;
	static const std::string NEIGHBOR;
};

struct NotificationStatus {
	static const std::string PENDING;
	static const std::string ACCEPTED;
	static const std::string REFUSED;
};

struct ServiceStatus {
	static const std::string OPEN;
	static const std::string CLOSED;
};

struct ServiceType {
	static const std::string OFFERED;
	static const std::string REQUESTED;
};