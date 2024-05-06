#include <cpr/cpr.h>
#include <gtest/gtest.h>
#include <unistd.h>
#include <cstdlib>
#include <nlohmann/json.hpp>
#include <pqxx/pqxx>
#include <string>
#include <vector>
#include "../common.h"

class GetServiceTest : public testing::Test {
	protected:
	std::string _service_id_;
	std::string _admin_id_;
	std::string _admin_token_;

	void register_admin() {
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

			_admin_token_ = token_value;
		} else {
			_admin_token_ = "";
		}

		auto json = nlohmann::json::parse(response.text);
		_admin_id_ = json["id"];
	}
	void SetUp() override {
		register_admin();
		std::string url_service = "http://backend:" + std::to_string(HTTP_PORT) + "/api/services";
		nlohmann::json new_service = {
			{"title", "nodeberiaexistir"},
			{"description", "some description 555555555"},
			{"price", 40},
			{"type", "offered"}};
		auto s_create = cpr::Post(cpr::Url{url_service}, cpr::Cookies{{"token", _admin_token_}}, cpr::Body{new_service.dump()}, cpr::Header{{"Content-Type", "application/json"}});
		auto json = nlohmann::json::parse(s_create.text);
		_service_id_ = json["id"];
		sleep(1);
	}

	void TearDown() override {
		clean_community_table();
		clean_user_table();
		// clean_service_table();
	}
};

// * void create_user_mock(nlohmann::json data);
//* void create_services_mock(std::string creator_id, int quantity);

// * crear usuario y obtener su token, no crear servicios y solo existe una comunidad
// ? testear que devuelve un 200 y un servies array vacio

// * crear usuario y obtener su token, existen servicios de ese usuario y solo existe una comunidas
// ? testear un 200 y services no es vacio, tiene servicios, comprobar que tiene las propiedades:

TEST_F(GetServiceTest, correct_id) {
	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/services/" + _service_id_;

	auto response = cpr::Get(cpr::Url{url}, cpr::Cookies{{"token", _admin_token_}}, cpr::Header{{"Content-Type", "application/json"}});
	auto json = nlohmann::json::parse(response.text);

	EXPECT_EQ(response.status_code, 200) << "Expected 200 status code for service got succesfully: ";
	ASSERT_TRUE(json.contains("service"));
	ASSERT_TRUE(json["service"].is_object()) << "Expected the 'service' key to contain an object.";
}

TEST_F(GetServiceTest, invalid_id) {
	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/services/non_real_service_id";

	auto response = cpr::Get(cpr::Url{url}, cpr::Cookies{{"token", _admin_token_}}, cpr::Header{{"Content-Type", "application/json"}});

	EXPECT_EQ(response.status_code, 400) << "expect 400 status code";

	auto json = nlohmann::json::parse(response.text);

	EXPECT_TRUE(json.contains("error"));
	EXPECT_EQ(json["error"], "id is invalid");
}

TEST_F(GetServiceTest, not_found_id) {
	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/services/11111111-1111-1111-1111-111111111111";

	auto response = cpr::Get(cpr::Url{url}, cpr::Cookies{{"token", _admin_token_}}, cpr::Header{{"Content-Type", "application/json"}});

	EXPECT_EQ(response.status_code, 404) << "expect 404 status code";

	auto json = nlohmann::json::parse(response.text);

	EXPECT_TRUE(json.contains("error"));
	EXPECT_EQ(json["error"], "service not found");
}
