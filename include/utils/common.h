#pragma once
#include <crow.h>
#include <db/connection_pool.h>
#include <middlewares/verify_jwt.h>
#include <bcrypt/BCrypt.hpp>
#include <cstdlib>
#include <map>
#include <string>


extern const std::string DB_NAME;
extern const std::string DB_USER;
extern const std::string DB_PASSWORD;
extern const std::string DB_HOST;
extern const int DB_PORT;
extern const int HTTP_PORT;
extern const std::string connection_string;

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
