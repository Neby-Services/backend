#include <crow.h>
#include <db/connection_pool.h>
#include <routes/auth_routes.h>
#include <routes/notification_routes.h>
#include <routes/service_routes.h>
#include <routes/user_routes.h>
#include <utils/common.h>
#include <cstdlib>
#include <format>
#include <iostream>
#include <pqxx/pqxx>
#include <string>

int main() {
	try {
		NebyApp app;

		initialize_auth_routes(app);
		initialize_user_routes(app);
		initialize_service_routes(app);
		initialize_notifications_routes(app);

		app.port(HTTP_PORT).multithreaded().run();
	} catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
		exit(1);
	}

	return 0;
}
