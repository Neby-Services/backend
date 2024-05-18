#include <cpr/cpr.h> 
#include <gtest/gtest.h>
#include <cstdlib>	// Para std::getenv
#include <nlohmann/json.hpp>
#include <pqxx/pqxx>
#include <string>
#include <vector>
#include "../common.h"
 
class LoginTest : public testing::Test {
	protected:
	std::string email_;
	std::string password_;

	void SetUp() override {
		email_ = "example@gmail.com";
		password_ = "P@ssw0rd!";

		std::string url_register = "http://backend:" + std::to_string(HTTP_PORT) + "/api/auth/register";
		nlohmann::json new_user = {
			{"email", email_},
			{"username", "tupapiloko"},
			{"password", password_},
			{"type", "admin"},  
			{"community_name", "example_community_name"}};

		auto r_register = cpr::Post(cpr::Url{url_register}, cpr::Body{new_user.dump()}, cpr::Header{{"Content-Type", "application/json"}});
	}
 
	void TearDown() override {
		clean_user_table();
		clean_community_table();
	}
};
 
class LoginErrorsTest : public testing::Test {
	protected:
	std::string email_;
	std::string password_;

	void SetUp() override {
		email_ = "example@gmail.com";
		password_ = "P@ssw0rd!";

		std::string url_register = "http://backend:" + std::to_string(HTTP_PORT) + "/api/auth/register";
		nlohmann::json new_user = {
			{"email", email_},
			{"username", "tupapiloko"},
			{"password", password_},
			{"type", "admin"},
			{"community_name", "example_community_name"}};

		auto r_register = cpr::Post(cpr::Url{url_register}, cpr::Body{new_user.dump()}, cpr::Header{{"Content-Type", "application/json"}});
	}

	void TearDown() override {
		clean_user_table();
		clean_community_table();
	}
};

TEST_F(LoginTest, correct_login) {
	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/auth/login";
	nlohmann::json post_data = {
		{"email", email_},
		{"password", password_},
	};

	auto response = cpr::Post(cpr::Url{url}, cpr::Body{post_data.dump()}, cpr::Header{{"Content-Type", "application/json"}});
	auto json = nlohmann::json::parse(response.text);

	EXPECT_EQ(response.status_code, 200);
	ASSERT_TRUE(json.contains("id"));
	ASSERT_TRUE(json["id"].is_string());

	std::string set_cookie_header = response.header["Set-Cookie"];
	size_t token_pos = set_cookie_header.find("token=");
	bool token_found = token_pos != std::string::npos;

	EXPECT_TRUE(token_found);
}

TEST_F(LoginErrorsTest, incorrect_password) {
	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/auth/login";
	nlohmann::json post_data = {
		{"email", email_},
		{"password", "Incorrect_password1"},
	};

	auto response = cpr::Post(cpr::Url{url}, cpr::Body{post_data.dump()}, cpr::Header{{"Content-Type", "application/json"}});
	auto json = nlohmann::json::parse(response.text);

	ASSERT_TRUE(json.contains("error"));
	EXPECT_EQ(json["error"], "invalid password");
	EXPECT_EQ(response.status_code, 400);
}

TEST_F(LoginErrorsTest, email_not_exists) {
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

TEST(LoginValidationInputTest, incorrect_email) {
	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/auth/login";

	// Lista de direcciones de correo electrónico incorrectas
	std::vector<std::string> incorrect_emails = {
		"example%@gmail.com",
		"example@domain.",
		"example@domain123",
		"example@domain,com",
		"example@domain.com.",
		"example@@domain.com",
		"example@domain..com",
		"example@@domain..com",
		"example@domain_com",
		"example@domain.com_com"};

	for (const auto& email : incorrect_emails) {
		nlohmann::json post_data = {
			{"email", email},
			{"password", "F!sh1ngR0ck5"}};

		auto response = cpr::Post(cpr::Url{url}, cpr::Body{post_data.dump()}, cpr::Header{{"Content-Type", "application/json"}});

		EXPECT_EQ(response.status_code, 400) << "Expected 400 status code for incorrect email: " << email;
	}
}

TEST(LoginValidationInputTest, incorrect_password) {
	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/auth/login";

	std::vector<std::string> incorrect_passwords = {
		"password", "12345678", "qwerty", "letmein", "abc123"};

	for (const auto& password : incorrect_passwords) {
		nlohmann::json post_data = {
			{"email", "example@gmail.com"},
			{"password", password}};

		auto response = cpr::Post(cpr::Url{url}, cpr::Body{post_data.dump()}, cpr::Header{{"Content-Type", "application/json"}});

		auto json = nlohmann::json::parse(response.text);

		EXPECT_EQ(response.status_code, 400) << "Expected 400 status code for incorrect password: " << password;
		EXPECT_TRUE(json.contains("error"));
		EXPECT_EQ(json["error"], "incorrect password");
	}
}

TEST(LoginValidationInputTest, missing_email) {
	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/auth/login";
	nlohmann::json post_data = {
		{"password", "Hola123."},
	};

	auto response = cpr::Post(cpr::Url{url}, cpr::Body{post_data.dump()}, cpr::Header{{"Content-Type", "application/json"}});

	auto json = nlohmann::json::parse(response.text);

	EXPECT_EQ(response.status_code, 404);
	EXPECT_TRUE(json.contains("error"));
	EXPECT_EQ(json["error"], "missing email field");
}

TEST(LoginValidationInputTest, missing_password) {
	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/auth/login";
	nlohmann::json post_data = {
		{"email", "example_email@gmail.com"},
	};

	auto response = cpr::Post(cpr::Url{url}, cpr::Body{post_data.dump()}, cpr::Header{{"Content-Type", "application/json"}});

	auto json = nlohmann::json::parse(response.text);

	EXPECT_EQ(response.status_code, 404);
	EXPECT_TRUE(json.contains("error"));
	EXPECT_EQ(json["error"], "missing password field");
}
