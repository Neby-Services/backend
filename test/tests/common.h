#ifndef COMMON_HPP
#define COMMON_HPP

#include <cpr/cpr.h>

#include <nlohmann/json.hpp>
#include <string>

extern const std::string DB_NAME;
extern const std::string DB_USER;
extern const std::string DB_PASSWORD;
extern const std::string DB_HOST;
extern const int DB_PORT;
extern const int HTTP_PORT;
extern const std::string token_get_all_services;
extern const std::string connection_string;

void clean_user_table();
void clean_community_table();

std::string create_user_test();

// ? Creates a new user and fetches their authentication JWT token from the cookie.
// ? This token is necessary for testing other routes requiring authentication and permissions.
std::string register_and_get_user_token();

//** mock services to test:
//*  	- GET /api/services?status=OPEN
void create_services();

#endif	// COMMON_HPP
