#include <cpr/cpr.h>
#include <gtest/gtest.h>
#include <cstdlib>
#include <nlohmann/json.hpp>
#include <pqxx/pqxx>
#include <string>
#include <vector>
#include "../common.h"

class GetServicesTest : public testing::Test {
	protected:
	std::string token1_;
	std::string _user_id1;
	std::string _user_id2;
	nlohmann::json user1_ = {
		{"email", "example@gmail.com"},
		{"username", "username"},
		{"password", "Hola123."},
		{"type", "admin"},
		{"community_name", "example_community_name"}};
	nlohmann::json user2_ = {
		{"email", "example2@gmail.com"},
		{"username", "username2"},
		{"password", "Hola123."},
		{"type", "admin"},
		{"community_name", "example_community_name"}};

	void SetUp() override {
		// create user1
		// create user 2
		// create services for user1
		// create services for user2
	}

	void TearDown() override {
		// clear_user_table();
		// clear_community_table();
		// clear_services_table();
	}
};

// * void create_user_mock(nlohmann::json data);
//* void create_services_mock(std::string creator_id, int quantity);

// * crear usuario y obtener su token, no crear servicios y solo existe una comunidad
// ? testear que devuelve un 200 y un servies array vacio

// * crear usuario y obtener su token, existen servicios de ese usuario y solo existe una comunidas
// ? testear un 200 y services no es vacio, tiene servicios, comprobar que tiene las propiedades:

TEST(GetServicesTest, first) {
	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/services";

	auto response = cpr::Get(cpr::Url{url}, cpr::Cookies{{"token", register_and_get_user_token()}});

	EXPECT_EQ(response.status_code, 200) << "expect 200 status code";

	auto json = nlohmann::json::parse(response.text);

	ASSERT_TRUE(json.contains("services"));

	ASSERT_TRUE(json["services"].is_array());

	clean_user_table();
	clean_community_table();
}
