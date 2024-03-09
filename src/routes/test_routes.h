#ifndef USER_ROUTES_H
#define USER_ROUTES_H

#include "crow.h"
#include <pqxx/pqxx>
#include "../controllers/test_controller.h"

void initialize_test_routes(crow::SimpleApp& app, pqxx::connection& db);

#endif
