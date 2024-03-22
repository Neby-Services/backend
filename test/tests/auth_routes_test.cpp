#include <cpr/cpr.h>
#include <gtest/gtest.h>

#include <cstdlib>	// Para std::getenv
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

#include "common.h"

// ** ---------- MISSING FIELDS ON REQ.BODY TESTS ---------- ** \\

TEST(REGISTER_MISSING_FIELDS, MissingEmail) {
	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/auth/register";

	nlohmann::json post_data = {
		{"password", "F!sh1ngR0ck5"},
		{"type", "admin"},
		{"username", "desssddddggdddddsssssdndis"},
		{"community_name", "test_jwsssssst_register"}};

	auto response = cpr::Post(cpr::Url{url}, cpr::Body{post_data.dump()}, cpr::Header{{"Content-Type", "application/json"}});

	EXPECT_EQ(response.status_code, 404) << "expect 404 status code with email missing";
}

TEST(REGISTER_MISSING_FIELDS, MissingUsername) {
	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/auth/register";

	nlohmann::json post_data = {
		{"email", "example@gmail.com"},
		{"password", "F!sh1ngR0ck5"},
		{"type", "admin"},
		{"community_name", "test_jwsssssst_register"}};

	auto response = cpr::Post(cpr::Url{url}, cpr::Body{post_data.dump()}, cpr::Header{{"Content-Type", "application/json"}});

	EXPECT_EQ(response.status_code, 404) << "expect 404 status code with username missing";
}

TEST(REGISTER_MISSING_FIELDS, MissingPassword) {
	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/auth/register";

	nlohmann::json post_data = {
		{"email", "example@gmail.com"},
		{"type", "admin"},
		{"username", "desssddddggdddddsssssdndis"},
		{"community_name", "test_jwsssssst_register"}};

	auto response = cpr::Post(cpr::Url{url}, cpr::Body{post_data.dump()}, cpr::Header{{"Content-Type", "application/json"}});

	EXPECT_EQ(response.status_code, 404) << "expect 404 status code with password missing";
}

TEST(REGISTER_MISSING_FIELDS, MissingType) {
	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/auth/register";

	nlohmann::json post_data = {
		{"email", "example@gmail.com"},
		{"password", "F!sh1ngR0ck5"},
		{"username", "example"},
		{"community_name", "test_jwsssssst_register"}};

	auto response = cpr::Post(cpr::Url{url}, cpr::Body{post_data.dump()}, cpr::Header{{"Content-Type", "application/json"}});

	EXPECT_EQ(response.status_code, 404) << "expect 404 status code with type missing";
}

TEST(REGISTER_MISSING_FIELDS, MissingCommunityName) {
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

TEST(REGISTER_MISSING_FIELDS, MissingCommunityCode) {
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

TEST(REGISTER_VALIDATIONS, IncorrectEmail) {
	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/auth/register";

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
			{"password", "F!sh1ngR0ck5"},
			{"type", "admin"},
			{"username", "example"},
			{"community_name", "example_community_name"}};

		auto response = cpr::Post(cpr::Url{url}, cpr::Body{post_data.dump()}, cpr::Header{{"Content-Type", "application/json"}});

		EXPECT_EQ(response.status_code, 400) << "Expected 400 status code for incorrect email: " << email;
	}
}