#ifndef AUTH_ROUTES_H
#define AUTH_ROUTES_H

#include <pqxx/pqxx>

#include "controllers/auth_controller.h"
#include "crow.h"

void initialize_auth_routes(crow::SimpleApp& app, pqxx::connection& db);

#endif
