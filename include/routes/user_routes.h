#include <pqxx/pqxx>
#include "crow.h"

#include "controllers/user_controller.h"
#include "utils/common.h"

void initialize_user_routes(NebyApp& app, pqxx::connection& db);