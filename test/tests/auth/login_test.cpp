#include <cpr/cpr.h>
#include <gtest/gtest.h>

#include <cstdlib>	// Para std::getenv
#include <nlohmann/json.hpp>
#include <pqxx/pqxx>
#include <string>
#include <vector>

#include "../common.h"

TEST(LOGIN_ERRORS, email_not_exists) {
	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/auth/login";

	nlohmann::json post_data = {
		{"email", "not_email_exist@gmail.com"},
		{"password", "F!sh1ngR0ck5"},
	};

	auto response = cpr::Post(cpr::Url{url}, cpr::Body{post_data.dump()}, cpr::Header{{"Content-Type", "application/json"}});

	auto json = nlohmann::json::parse(response.text);

	ASSERT_TRUE(json.contains("error"));

	EXPECT_EQ(json["error"], "no user found with this email");

	EXPECT_EQ(response.status_code, 404);
}

TEST(LOGIN_ERRORS, incorrect_password) {
	// * Before_test
	std::string url_register = "http://backend:" + std::to_string(HTTP_PORT) + "/api/auth/register";

	std::string email = "example@gmail.com";
	std::string password = "P@ssw0rd!";

	nlohmann::json new_user = {
		{"email", email},
		{"username", "tupapiloko"},
		{"password", password},
		{"type", "admin"},
		{"community_name", "example_community_name"}};

	auto r_register = cpr::Post(cpr::Url{url_register}, cpr::Body{new_user.dump()}, cpr::Header{{"Content-Type", "application/json"}});

	// * Test

	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/auth/login";

	nlohmann::json post_data = {
		{"email", email},
		{"password", "Incorrect_password1"},
	};

	auto response = cpr::Post(cpr::Url{url}, cpr::Body{post_data.dump()}, cpr::Header{{"Content-Type", "application/json"}});

	auto json = nlohmann::json::parse(response.text);

	ASSERT_TRUE(json.contains("error"));

	EXPECT_EQ(json["error"], "invalid password");

	EXPECT_EQ(response.status_code, 400);

	// * After_test
	clean_user_table();
	clean_community_table();
}

TEST(LOGIN, correct_login) {
	// * Before_test
	std::string url_register = "http://backend:" + std::to_string(HTTP_PORT) + "/api/auth/register";

	std::string email = "example@gmail.com";
	std::string password = "P@ssw0rd!";

	nlohmann::json new_user = {
		{"email", email},
		{"username", "tupapiloko"},
		{"password", password},
		{"type", "admin"},
		{"community_name", "example_community_name"}};

	auto r_register = cpr::Post(cpr::Url{url_register}, cpr::Body{new_user.dump()}, cpr::Header{{"Content-Type", "application/json"}});

	// * Test

	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/auth/login";

	nlohmann::json post_data = {
		{"email", email},
		{"password", password},
	};

	auto response = cpr::Post(cpr::Url{url}, cpr::Body{post_data.dump()}, cpr::Header{{"Content-Type", "application/json"}});

	auto json = nlohmann::json::parse(response.text);

	EXPECT_EQ(response.status_code, 200);
	ASSERT_TRUE(json.contains("id"));
	ASSERT_TRUE(json["id"].is_string());

	std::string set_cookie_header = response.header["Set-Cookie"];

	// Buscar la cookie "token" dentro del encabezado "Set-Cookie"
	size_t token_pos = set_cookie_header.find("token=");
	bool token_found = token_pos != std::string::npos;

	// Verificar que se encontró la cookie "token"
	EXPECT_TRUE(token_found);

	// * After_test
	clean_user_table();
	clean_community_table();
}