#include <cpr/cpr.h>
#include <gtest/gtest.h>
#include <cstdlib>
#include <nlohmann/json.hpp>
#include <pqxx/pqxx>
#include <string>
#include <vector>
#include "../common.h"

class GetServicesSelfTest : public testing::Test {
	protected:
	std::string _service_id_;
	std::string _admin_id_;
	std::string _admin_id2_;
	std::string _admin_token_;
	std::string _admin_token2_;

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
	void register_admin2() {
		std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/auth/register";

		nlohmann::json new_user = {
			{"email", "example2@gmail.com"},
			{"username", "example2"},
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

			_admin_token2_ = token_value;
		} else {
			_admin_token2_ = "";
		}

		auto json = nlohmann::json::parse(response.text);
		_admin_id2_ = json["id"];
	}
	void SetUp() override {
		register_admin();
		std::string url_service = "http://backend:" + std::to_string(HTTP_PORT) + "/api/services";
		nlohmann::json new_service1 = {
			{"title", "nodeberiaexistir1"},
			{"description", "some description 1"},
			{"price", 40},
			{"type", "offered"}};
		auto s_create1 = cpr::Post(cpr::Url{url_service}, cpr::Cookies{{"token", _admin_token_}}, cpr::Body{new_service1.dump()}, cpr::Header{{"Content-Type", "application/json"}});
		auto json = nlohmann::json::parse(s_create1.text);
		_service_id_ = json["id"];
		nlohmann::json new_service2 = {
			{"title", "nodeberiaexistir2"},
			{"description", "some description 2"},
			{"price", 50},
			{"type", "offered"}};
		auto s_create2 = cpr::Post(cpr::Url{url_service}, cpr::Cookies{{"token", _admin_token_}}, cpr::Body{new_service2.dump()}, cpr::Header{{"Content-Type", "application/json"}});
		nlohmann::json new_service3 = {
			{"title", "nodeberiaexistir3"},
			{"description", "some description 3"},
			{"price", 60},
			{"type", "offered"}};
		auto s_create3 = cpr::Post(cpr::Url{url_service}, cpr::Cookies{{"token", _admin_token_}}, cpr::Body{new_service3.dump()}, cpr::Header{{"Content-Type", "application/json"}});
		nlohmann::json new_service4 = {
			{"title", "nodeberiaexistir4"},
			{"description", "some description 4"},
			{"price", 70},
			{"type", "offered"}};
		auto s_create4 = cpr::Post(cpr::Url{url_service}, cpr::Cookies{{"token", _admin_token_}}, cpr::Body{new_service4.dump()}, cpr::Header{{"Content-Type", "application/json"}});
		sleep(1);
		register_admin2();
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

TEST_F(GetServicesSelfTest, get_all_services_self) {
	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/services/self";

	auto response = cpr::Get(cpr::Url{url}, cpr::Cookies{{"token", _admin_token_}}, cpr::Header{{"Content-Type", "application/json"}});

	EXPECT_EQ(response.status_code, 200) << "expect 200 status code";

	auto json = nlohmann::json::parse(response.text);

	ASSERT_TRUE(json.contains("self_services"));

	ASSERT_TRUE(json["self_services"].is_array());
	ASSERT_EQ(json["self_services"].size(), 4);
}

TEST_F(GetServicesSelfTest, get_all_services_self_closed) {
	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/services/self?status=closed";

	auto response = cpr::Get(cpr::Url{url}, cpr::Cookies{{"token", _admin_token_}}, cpr::Header{{"Content-Type", "application/json"}});

	EXPECT_EQ(response.status_code, 200) << "expect 200 status code";

	auto json = nlohmann::json::parse(response.text);

	ASSERT_TRUE(json.contains("self_services"));

	ASSERT_TRUE(json["self_services"].is_array());
	for (const auto& self_service : json["self_services"]) {
		ASSERT_TRUE(self_service.contains("status"));
		ASSERT_EQ(self_service["status"], "closed");
	}
}

TEST_F(GetServicesSelfTest, get_all_services_self_open) {
	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/services/self?status=open";

	auto response = cpr::Get(cpr::Url{url}, cpr::Cookies{{"token", _admin_token_}}, cpr::Header{{"Content-Type", "application/json"}});

	EXPECT_EQ(response.status_code, 200) << "expect 200 status code";

	auto json = nlohmann::json::parse(response.text);

	ASSERT_TRUE(json.contains("self_services"));

	ASSERT_TRUE(json["self_services"].is_array());
	for (const auto& self_service : json["self_services"]) {
		ASSERT_TRUE(self_service.contains("status"));
		ASSERT_EQ(self_service["status"], "open");
	}
}

TEST_F(GetServicesSelfTest, get_all_services_self_wrong_status) {
	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/services/self?status=NON_EXISTENT_STATUS";

	auto response = cpr::Get(cpr::Url{url}, cpr::Cookies{{"token", _admin_token_}}, cpr::Header{{"Content-Type", "application/json"}});

	EXPECT_EQ(response.status_code, 400) << "status not valid value";

	auto json = nlohmann::json::parse(response.text);

	EXPECT_TRUE(json.contains("error"));
	EXPECT_EQ(json["error"], "status not valid value");
	for (const auto& self_service : json["self_services"]) {
		ASSERT_TRUE(self_service.contains("status"));
		ASSERT_EQ(self_service["status"], "closed");
	}
}

TEST_F(GetServicesSelfTest, get_all_services_self_empty_list) {
	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/services/self";

	auto response = cpr::Get(cpr::Url{url}, cpr::Cookies{{"token", _admin_token2_}}, cpr::Header{{"Content-Type", "application/json"}});

	EXPECT_EQ(response.status_code, 200) << "expect 200 status code";

	auto json = nlohmann::json::parse(response.text);

	ASSERT_TRUE(json.contains("self_services"));

	ASSERT_TRUE(json["self_services"].is_array());
	ASSERT_TRUE(json["sel_services"].empty());
}
