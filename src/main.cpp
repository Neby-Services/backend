#include <crow.h>
#include <db/connection_pool.h>
#include <routes/auth_routes.h>
#include <routes/notification_routes.h>
#include <routes/service_routes.h>
#include <routes/user_routes.h>
#include <routes/user_achievement_routes.h>
#include <routes/rating_routes.h>
#include <middlewares/index_middlewares.h>
#include <cstdlib>
#include <format>
#include <iostream>
#include <pqxx/pqxx>
#include <string>

int main() {
	try {
		NebyApp app;
		ConnectionPool pool(connection_string, 5);

		initialize_auth_routes(app, pool);
		initialize_user_routes(app, pool);
		initialize_service_routes(app, pool);
		initialize_notifications_routes(app, pool);
		initialize_user_achievement_routes(app, pool);

		app.port(HTTP_PORT).multithreaded().run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		exit(1);
	}

	return 0;
}
