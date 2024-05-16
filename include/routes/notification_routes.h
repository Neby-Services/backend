#pragma once 

#include <controllers/notification_controller.h>
#include <crow.h>
#include <middlewares/index_middlewares.h>
#include <db/connection_pool.h>
#include <middlewares/verify_jwt.h> 	
#include <db/connection_pool.h>
#include <utils/common.h>
#include <pqxx/pqxx>

void initialize_notifications_routes(NebyApp& app);
