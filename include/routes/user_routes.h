#pragma once

#include <controllers/user_controller.h>
#include <crow.h>
#include <middlewares/verify_jwt.h>
#include <utils/common.h>
#include <pqxx/pqxx>

void initialize_user_routes(NebyApp& app, pqxx::connection& db);
