#pragma once

#include "crow.h"
#include <pqxx/pqxx>

#include "utils/common.h"
#include "controllers/auth_controller.h"
#include <middlewares/verify_jwt.h>

void initialize_auth_routes(NebyApp& app, pqxx::connection& db);
