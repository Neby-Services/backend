#include <pqxx/pqxx>

#include "controllers/test_controller.h"
#include "crow.h"

void initialize_user_routes(crow::SimpleApp& app, pqxx::connection& db);