#ifndef USER_ROUTES_H
#define USER_ROUTES_H

#include "crow.h"
#include "../controllers/test_controller.h"

void initialize_test_routes(crow::SimpleApp& app);

#endif
