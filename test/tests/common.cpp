#include "common.h"

#include <cstdlib>	// Para std::getenv
#include <format>
#include <iostream>
#include <pqxx/pqxx>

const int HTTP_PORT = std::stoi(std::getenv("HTTP_PORT"));
const std::string DB_NAME = std::string(std::getenv("DB_NAME"));
const std::string DB_USER = std::string(std::getenv("DB_USER"));
const std::string DB_PASSWORD = std::string(std::getenv("DB_PASSWORD"));
const std::string DB_HOST = std::string(std::getenv("DB_HOST"));
const int DB_PORT = std::stoi(std::getenv("DB_PORT"));
const std::string token_get_all_services = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXUyJ9.eyJpZCI6IjZjZWM1NzA0LTY0MjMtNDUyZC1iMDNjLTdmMDFiY2NhMWVjYyIsImlzcyI6ImF1dGgwIiwidHlwZSI6ImFkbWluIn0.PNlBpdYsY5Md-wxugYW6J2Sd3pD3HlrkKNxWrW2fs7A";
const std::string connection_string = std::format("dbname={} user={} password={} host={} port={}", DB_NAME, DB_USER, DB_PASSWORD, DB_HOST, DB_PORT);

void clean_user_table() {
	try {
		pqxx::connection conn(connection_string);

		if (conn.is_open()) {
			pqxx::work txn(conn);

			txn.exec("DELETE FROM users");

			txn.commit();

		} else {
			std::cout << "Error al conectar a la base de datos." << std::endl;
		}
	} catch (const std::exception& e) {
		std::cout << "Error de excepción: " << e.what() << std::endl;
	}
}

void clean_community_table() {
	try {
		pqxx::connection conn(connection_string);

		if (conn.is_open()) {
			pqxx::work txn(conn);

			txn.exec("DELETE FROM communities");

			txn.commit();

		} else {
			std::cout << "Error al conectar a la base de datos." << std::endl;
		}
	} catch (const std::exception& e) {
		std::cout << "Error de excepción: " << e.what() << std::endl;
	}
}

std::string create_user_test() {
	pqxx::connection conn(connection_string);

	if (conn.is_open()) {
		try {
			pqxx::work txn(conn);

			std::string username = "test_user";
			std::string email = "test@example.com";
			std::string password = "password123";
			std::string image_url = "https://example.com/image.jpg";
			std::string type = "neighbor";	// or "admin" depending on the user's type

			pqxx::result result = txn.exec_params("INSERT INTO users (username, email, password, image_url, type) VALUES ($1, $2, $3, $4, $5) RETURNING id",
												  username, email, password, image_url, type);

			txn.commit();

			return result[0][0].as<std::string>();
		} catch (const std::exception& e) {
			std::cerr << "Error creating user: " << e.what() << std::endl;
		}
	} else {
		std::cerr << "Error connecting to the database." << std::endl;
	}

	return "";
}

void create_services() {
	std::string user_id = create_user_test();

	pqxx::connection conn(connection_string);

	if (conn.is_open()) {
		try {
			pqxx::work txn(conn);

			for (int i = 1; i <= 5; ++i) {
				std::string title = "Service " + std::to_string(i);
				std::string description = "Description of service " + std::to_string(i);
				int price = 100 * i;
				std::string type = "REQUESTED";

				txn.exec_params("INSERT INTO services (creator_id, title, description, price, type) VALUES ($1, $2, $3, $4, $5)",
								user_id, title, description, price, type);
			}

			txn.commit();

			std::cout << "Example services created successfully." << std::endl;
		} catch (const std::exception& e) {
			std::cerr << "Error creating example services: " << e.what() << std::endl;
		}
	} else {
		std::cerr << "Error connecting to the database." << std::endl;
	}
}

std::string register_and_get_user_token() {
	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/auth/register";

	nlohmann::json new_user = {
		{"email", "example@gmail.com"},
		{"username", "example"},
		{"password", "P@ssw0rd!"},
		{"type", "admin"},
		{"community_name", "example_community_name"}};

	auto response = cpr::Post(cpr::Url{url}, cpr::Body{new_user.dump()}, cpr::Header{{"Content-Type", "application/json"}});

	std::string set_cookie_header = response.header["Set-Cookie"];

	size_t token_pos = set_cookie_header.find("token=");
	if (token_pos != std::string::npos) {
		size_t token_start = token_pos + 6;
		size_t token_end = set_cookie_header.find(";", token_start);
		std::string token_value = set_cookie_header.substr(token_start, token_end - token_start);

		return token_value;
	} else {
		return "";
	}
}
