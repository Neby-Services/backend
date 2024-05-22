#pragma once

#include <middlewares/index_middlewares.h>
#include <controllers/auth_controller.h>
#include <crow.h>
#include <middlewares/verify_jwt.h>
#include <utils/common.h>
#include <pqxx/pqxx>
#include <db/connection_pool.h>

void initialize_auth_routes(NebyApp& app, ConnectionPool & pool);
