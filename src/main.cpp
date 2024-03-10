#include <cstdlib>
#include <format>
#include <iostream>
#include <pqxx/pqxx>
#include <string>

#include "crow.h"
#include "routes/auth_routes.h"
#include "routes/test_routes.h"

int main() {
	try {
		int HTTP_PORT = std::stoi(std::getenv("HTTP_PORT"));
		std::string DB_NAME = std::string(std::getenv("DB_NAME"));
		std::string DB_USER = std::string(std::getenv("DB_USER"));
		std::string DB_PASSWORD = std::string(std::getenv("DB_PASSWORD"));
		std::string DB_HOST = std::string(std::getenv("DB_HOST"));
		std::string DB_PORT = std::string(std::getenv("DB_PORT"));

		crow::SimpleApp app;
		std::string connection_string = std::format("dbname={} user={} password={} host={} port={}", DB_NAME, DB_USER, DB_PASSWORD, DB_HOST, DB_PORT);
		pqxx::connection conn(connection_string);

		if (conn.is_open()) {
			std::cout << "Opened database successfully: " << conn.dbname() << std::endl;
		} else {
			std::cout << "Can't open database" << std::endl;
			exit(1);
		}

		initialize_auth_routes(app, conn);
		initialize_test_routes(app, conn);

		app.port(HTTP_PORT).multithreaded().run();
		conn.disconnect();
	} catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
		exit(1);
	}

	return 0;
}
