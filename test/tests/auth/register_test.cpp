#include <cpr/cpr.h>
#include <gtest/gtest.h>
#include <cstdlib>	// Para std::getenv
#include <nlohmann/json.hpp>
#include <pqxx/pqxx>
#include <string>
#include <vector>
#include "../common.h"

class RegisterValidations : public ::testing::Test {
	protected:
	void TearDown() override {
		clean_user_table();
		clean_community_table();
	}
};

class RegisterGeneralErrors : public ::testing::Test {
	protected:
	void TearDown() override {
		clean_community_table();
		clean_user_table();
	}
};

// ** ---------- MISSING FIELDS ON REQ.BODY TESTS ---------- ** \\

TEST(REGISTER_MISSING_FIELDS, missing_email) {
	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/auth/register";

	nlohmann::json post_data = {
		{"password", "F!sh1ngR0ck5"},
		{"type", "admin"},
		{"username", "desssddddggdddddsssssdndis"},
		{"community_name", "test_jwsssssst_register"}};

	auto response = cpr::Post(cpr::Url{url}, cpr::Body{post_data.dump()}, cpr::Header{{"Content-Type", "application/json"}});

	EXPECT_EQ(response.status_code, 404) << "expect 404 status code with email missing";
}

TEST(REGISTER_MISSING_FIELDS, missing_username) {
	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/auth/register";

	nlohmann::json post_data = {
		{"email", "example@gmail.com"},
		{"password", "F!sh1ngR0ck5"},
		{"type", "admin"},
		{"community_name", "test_jwsssssst_register"}};

	auto response = cpr::Post(cpr::Url{url}, cpr::Body{post_data.dump()}, cpr::Header{{"Content-Type", "application/json"}});

	EXPECT_EQ(response.status_code, 404) << "expect 404 status code with username missing";
}

TEST(REGISTER_MISSING_FIELDS, missing_password) {
	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/auth/register";

	nlohmann::json post_data = {
		{"email", "example@gmail.com"},
		{"type", "admin"},
		{"username", "desssddddggdddddsssssdndis"},
		{"community_name", "test_jwsssssst_register"}};

	auto response = cpr::Post(cpr::Url{url}, cpr::Body{post_data.dump()}, cpr::Header{{"Content-Type", "application/json"}});

	EXPECT_EQ(response.status_code, 404) << "expect 404 status code with password missing";
}

TEST(REGISTER_MISSING_FIELDS, missing_type) {
	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/auth/register";

	nlohmann::json post_data = {
		{"email", "example@gmail.com"},
		{"password", "F!sh1ngR0ck5"},
		{"username", "example"},
		{"community_name", "test_jwsssssst_register"}};

	auto response = cpr::Post(cpr::Url{url}, cpr::Body{post_data.dump()}, cpr::Header{{"Content-Type", "application/json"}});

	EXPECT_EQ(response.status_code, 404) << "expect 404 status code with type missing";
}

TEST(REGISTER_MISSING_FIELDS, missing_community_name) {
	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/auth/register";

	nlohmann::json post_data = {
		{"email", "example@gmail.com"},
		{"password", "F!sh1ngR0ck5"},
		{"type", "admin"},
		{"username", "example"},
	};

	auto response = cpr::Post(cpr::Url{url}, cpr::Body{post_data.dump()}, cpr::Header{{"Content-Type", "application/json"}});

	EXPECT_EQ(response.status_code, 404) << "expect 404 status code with community_name missing";
}

TEST(REGISTER_MISSING_FIELDS, missing_community_code) {
	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/auth/register";

	nlohmann::json post_data = {
		{"email", "example@gmail.com"},
		{"password", "F!sh1ngR0ck5"},
		{"type", "neighbor"},
		{"username", "example"},
	};

	auto response = cpr::Post(cpr::Url{url}, cpr::Body{post_data.dump()}, cpr::Header{{"Content-Type", "application/json"}});

	EXPECT_EQ(response.status_code, 404) << "expect 404 status code with community_code missing";
}

// ** ---------- VALIDATION REQ.BODY FIELDS TESTS ---------- ** \\

TEST_F(RegisterValidations, correct_email) {
	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/auth/register";

	std::vector<std::string> correct_emails = {
		"user@example.com",
		"user123@example.com",
		"user.name@example.com",
		"user_name@example.com",
		"user+name@example.com",
		"user-name@example.com",
		"user@example.co",
		"user@example.co.uk",
		"user@example.xyz"};

	for (const auto& email : correct_emails) {
		nlohmann::json post_data = {
			{"email", email},
			{"password", "F!sh1ngR0ck5"},
			{"type", "admin"},
			{"username", "example"},
			{"community_name", "example_community_name"}};

		auto response = cpr::Post(cpr::Url{url}, cpr::Body{post_data.dump()}, cpr::Header{{"Content-Type", "application/json"}});

		EXPECT_EQ(response.status_code, 201) << "Expected 201 status code for correct email: " << email;

		clean_community_table();
		clean_user_table();
	}
}

TEST_F(RegisterValidations, incorrect_email) {
	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/auth/register";

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
			{"password", "F!sh1ngR0ck5"},
			{"type", "admin"},
			{"username", "example"},
			{"community_name", "example_community_name"}};

		auto response = cpr::Post(cpr::Url{url}, cpr::Body{post_data.dump()}, cpr::Header{{"Content-Type", "application/json"}});

		EXPECT_EQ(response.status_code, 400) << "Expected 400 status code for incorrect email: " << email;
	}
}

TEST_F(RegisterValidations, correct_password) {
	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/auth/register";

	std::vector<std::string> incorrect_passwords = {
		"Tr0ub4dor&3",
		"P@ssw0rd!",
		"S3cur3P@ss",
		"B1gB@ngTh3ory",
		"F!sh1ngR0ck5"};

	for (const auto& password : incorrect_passwords) {
		nlohmann::json post_data = {
			{"email", "example@gmail.com"},
			{"username", "example"},
			{"password", password},
			{"type", "admin"},
			{"community_name", "example_community_name"}};

		auto response = cpr::Post(cpr::Url{url}, cpr::Body{post_data.dump()}, cpr::Header{{"Content-Type", "application/json"}});

		EXPECT_EQ(response.status_code, 201) << "Expected 201 status code for incorrect password: " << password;
		clean_community_table();
		clean_user_table();
	}
}

TEST_F(RegisterValidations, incorrect_password) {
	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/auth/register";

	std::vector<std::string> incorrect_passwords = {
		"password", "12345678", "qwerty", "letmein", "abc123"};

	for (const auto& password : incorrect_passwords) {
		nlohmann::json post_data = {
			{"email", "example@gmail.com"},
			{"username", "example"},
			{"password", password},
			{"type", "admin"},
			{"community_name", "example_community_name"}};

		auto response = cpr::Post(cpr::Url{url}, cpr::Body{post_data.dump()}, cpr::Header{{"Content-Type", "application/json"}});

		EXPECT_EQ(response.status_code, 400) << "Expected 400 status code for incorrect password: " << password;
	}
}

TEST_F(RegisterValidations, incorrect_username) {
	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/auth/register";

	std::vector<std::string> incorrect_usernames = {
		"Invalid!User",
		"SpacesUser ",
		"Short",
		"LongUsernameWithTooManyCharacters",
		"Invalid Spaces"};

	for (const auto& username : incorrect_usernames) {
		nlohmann::json post_data = {
			{"email", "example@gmail.com"},
			{"username", username},
			{"password", "P@ssw0rd!"},
			{"type", "admin"},
			{"community_name", "example_community_name"}};

		auto response = cpr::Post(cpr::Url{url}, cpr::Body{post_data.dump()}, cpr::Header{{"Content-Type", "application/json"}});

		EXPECT_EQ(response.status_code, 400) << "Expected 400 status code for incorrect username: " << username;
	}
}

TEST_F(RegisterValidations, correct_username) {
	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/auth/register";

	std::vector<std::string> incorrect_usernames = {
		"SecureUser1",
		"Usuario123",
		"AlphaBeta45",
		"GoodPassword23",
		"ValidUsername"};

	for (const auto& username : incorrect_usernames) {
		nlohmann::json post_data = {
			{"email", "example@gmail.com"},
			{"username", username},
			{"password", "P@ssw0rd!"},
			{"type", "admin"},
			{"community_name", "example_community_name"}};

		auto response = cpr::Post(cpr::Url{url}, cpr::Body{post_data.dump()}, cpr::Header{{"Content-Type", "application/json"}});

		EXPECT_EQ(response.status_code, 201) << "Expected 201 status code for incorrect username: " << username;
		clean_community_table();
		clean_user_table();
	}
}

TEST_F(RegisterValidations, incorrect_type) {
	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/auth/register";

	std::string incorrect_type = "type_error";

	nlohmann::json post_data = {
		{"email", "example@gmail.com"},
		{"username", "example"},
		{"password", "P@ssw0rd!"},
		{"type", incorrect_type},
		{"community_name", "example_community_name"}};

	auto response = cpr::Post(cpr::Url{url}, cpr::Body{post_data.dump()}, cpr::Header{{"Content-Type", "application/json"}});

	EXPECT_EQ(response.status_code, 400) << "Expected 400 status code for incorrect username: " << incorrect_type;
}

// ** ---------- GENERAL ERRORS TESTS ---------- ** \\


TEST_F(RegisterGeneralErrors, user_already_exists) {
	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/auth/register";

	nlohmann::json new_user = {
		{"email", "example@gmail.com"},
		{"username", "example"},
		{"password", "P@ssw0rd!"},
		{"type", "admin"},
		{"community_name", "example_community_name"}};

	nlohmann::json user_exist_email = {
		{"email", "example@gmail.com"},
		{"username", "example1"},
		{"password", "P@ssw0rd!"},
		{"type", "admin"},
		{"community_name", "example_community_name"}};
	nlohmann::json user_exist_username = {
		{"email", "example@gmail.com"},
		{"username", "example1"},
		{"password", "P@ssw0rd!"},
		{"type", "admin"},
		{"community_name", "example_community_name"}};

	auto response = cpr::Post(cpr::Url{url}, cpr::Body{new_user.dump()}, cpr::Header{{"Content-Type", "application/json"}});

	auto response_email = cpr::Post(cpr::Url{url}, cpr::Body{user_exist_email.dump()}, cpr::Header{{"Content-Type", "application/json"}});

	auto json = nlohmann::json::parse(response_email.text);

	ASSERT_TRUE(json.contains("error"));
	std::string error_message_email = json["error"];
	EXPECT_EQ(error_message_email, "email already in use");
	EXPECT_EQ(response_email.status_code, 400);

	auto response_username = cpr::Post(cpr::Url{url}, cpr::Body{user_exist_username.dump()}, cpr::Header{{"Content-Type", "application/json"}});

	json = nlohmann::json::parse(response_username.text);

	ASSERT_TRUE(json.contains("error"));
	std::string error_message_username = json["error"];
	EXPECT_EQ(error_message_username, "email already in use");
	EXPECT_EQ(response_username.status_code, 400);
}

TEST_F(RegisterGeneralErrors, correct_signup) {
	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/auth/register";

	nlohmann::json new_user = {
		{"email", "example@gmail.com"},
		{"username", "example"},
		{"password", "P@ssw0rd!"},
		{"type", "admin"},
		{"community_name", "example_community_name"}};

	auto response = cpr::Post(cpr::Url{url}, cpr::Body{new_user.dump()}, cpr::Header{{"Content-Type", "application/json"}});

	EXPECT_EQ(response.status_code, 201);

	auto json = nlohmann::json::parse(response.text);

	ASSERT_TRUE(json.contains("id"));

	std::string set_cookie_header = response.header["Set-Cookie"];

	size_t token_pos = set_cookie_header.find("token=");
	bool token_found = token_pos != std::string::npos;

	EXPECT_TRUE(token_found);
}

TEST_F(RegisterGeneralErrors, community_not_exists) {
	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/auth/register";

	nlohmann::json new_user = {
		{"email", "example@gmail.com"},
		{"username", "example"},
		{"password", "P@ssw0rd!"},
		{"type", "neighbor"},
		{"community_code", "example_community_code"}};

	auto response = cpr::Post(cpr::Url{url}, cpr::Body{new_user.dump()}, cpr::Header{{"Content-Type", "application/json"}});

	EXPECT_EQ(response.status_code, 404);

	auto json = nlohmann::json::parse(response.text);

	ASSERT_TRUE(json.contains("error"));
	std::string error_message_username = json["error"];
	EXPECT_EQ(error_message_username, "community does not exists");
}
