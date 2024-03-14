#include <cpr/cpr.h>
#include <gtest/gtest.h>

#include <cstdlib>	// Para std::getenv
#include <nlohmann/json.hpp>
#include <string>

int HTTP_PORT = std::stoi(std::getenv("HTTP_PORT"));

int sum(int a, int b) {
	return a + b;
}

TEST(test1, PositiveNumbers) {
	EXPECT_EQ(sum(2, 3), 5);
}

TEST(IntegrationTest, HttpRequest) {
	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/users";

	auto response = cpr::Get(cpr::Url{url});

	EXPECT_EQ(response.status_code, 200);

	auto json = nlohmann::json::parse(response.text);

	ASSERT_TRUE(json.contains("users"));
	ASSERT_TRUE(json["users"].is_array());

	for (const auto& user : json["users"]) {
		ASSERT_TRUE(user.contains("id"));
		ASSERT_TRUE(user.contains("image_url"));
		ASSERT_TRUE(user.contains("email"));
		ASSERT_TRUE(user.contains("username"));
		ASSERT_TRUE(user.contains("balance"));
		ASSERT_TRUE(user.contains("type"));
	}
}