#pragma once

#include <controllers/auth_controller.h>
#include <crow.h>
#include <middlewares/verify_jwt.h>
#include <utils/common.h>
#include <pqxx/pqxx>

void initialize_auth_routes(NebyApp& app, pqxx::connection& db);
