#pragma once

#include <middlewares/verify_jwt.h>

#include <pqxx/pqxx>

#include <controllers/service_controller.h>
#include <crow.h>
#include <utils/common.h>

void initialize_service_routes(NebyApp& app, pqxx::connection& db);