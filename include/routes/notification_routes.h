#pragma once

#include <controllers/notification_controller.h>
#include <crow.h>
#include <db/connection_pool.h>
#include <middlewares/handle_achievements.h>
#include <middlewares/index_middlewares.h>
#include <middlewares/verify_jwt.h>
#include <utils/common.h>
#include <pqxx/pqxx>

void initialize_notifications_routes(NebyApp& app);
