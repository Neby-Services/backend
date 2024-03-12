#include <pqxx/pqxx>
#include "crow.h"

#include "controllers/auth_controller.h"
#include "utils/common.h"

void initialize_auth_routes(NebyApp& app, pqxx::connection& db);
