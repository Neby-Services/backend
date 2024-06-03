#pragma once

#include <controllers/user_achievement_controller.h>
#include <crow.h>
#include <db/connection_pool.h>
#include <middlewares/verify_jwt.h>
#include <middlewares/handle_achievements.h>
#include <utils/common.h>
#include <middlewares/index_middlewares.h>
#include <pqxx/pqxx>

void initialize_user_achievement_routes(NebyApp& app, ConnectionPool & pool);
