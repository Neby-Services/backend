#ifndef USER_ROUTES_H
#define USER_ROUTES_H

#include <pqxx/pqxx>

#include "../controllers/test_controller.h"
#include "crow.h"

void initialize_test_routes(crow::SimpleApp& app, pqxx::connection& db);

#endif
