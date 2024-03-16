#pragma once

#include <pqxx/pqxx>

#include "controllers/auth_controller.h"
#include "crow.h"
#include "utils/common.h"

void initialize_auth_routes(NebyApp& app, pqxx::connection& db);
