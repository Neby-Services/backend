#include <iostream>
#include <cstdlib>
#include <format>
#include <string>
#include <pqxx/pqxx>
#include "crow.h"

#include "./routes/test_routes.h"

int main() {
	try {
		int HTTP_PORT = std::stoi(std::getenv("HTTP_PORT"));
		std::string DB_NAME = std::string(std::getenv("DB_NAME"));
		std::string DB_USER = std::string(std::getenv("DB_USER"));
		std::string DB_PASSWORD = std::string(std::getenv("DB_PASSWORD"));
		std::string DB_HOST = std::string(std::getenv("DB_HOST"));
		std::string DB_PORT = std::string(std::getenv("DB_PORT"));

		crow::SimpleApp app;
		initialize_test_routes(app);
		app.port(HTTP_PORT).multithreaded().run();
	} catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
		exit(1);
	}

	return 0;
}
