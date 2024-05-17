#pragma once

#include <controllers/service_controller.h>
#include <crow.h>
#include <middlewares/verify_jwt.h>
#include <middlewares/handle_achievements.h>
#include <db/connection_pool.h>
#include <utils/common.h>
#include <pqxx/pqxx>

#include <middlewares/index_middlewares.h>
void initialize_service_routes(NebyApp& app);
