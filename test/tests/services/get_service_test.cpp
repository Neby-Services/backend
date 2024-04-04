#include <cpr/cpr.h>
#include <gtest/gtest.h>

#include <cstdlib>	// Para std::getenv
#include <nlohmann/json.hpp>
#include <pqxx/pqxx>
#include <string>
#include <vector>

#include "../common.h"

TEST(GET_SERVICES, first) {
	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/services";

	auto response = cpr::Get(cpr::Url{url}, cpr::Cookies{{"token", register_and_get_user_token()}});

	std::cout << " test -> " << response.text << std::endl;

	EXPECT_EQ(response.status_code, 200) << "expect 200 status code";

	auto json = nlohmann::json::parse(response.text);

	ASSERT_TRUE(json.contains("services"));

	ASSERT_TRUE(json["services"].is_array());

	clean_user_table();
	clean_community_table();
}

