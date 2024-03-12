#include <pqxx/pqxx>

#include "controllers/test_controller.h"
#include "crow.h"
#include "utils/common.h"

void initialize_test_routes(NebyApp& app, pqxx::connection& db);
