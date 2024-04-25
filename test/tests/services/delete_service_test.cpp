#include <cpr/cpr.h>
#include <gtest/gtest.h>

#include <cstdlib>	// Para std::getenv
#include <nlohmann/json.hpp>
#include <pqxx/pqxx>
#include <string>
#include <vector>

#include "../common.h"
/*

TEST if user not authentication with jwt
status_code = 401

 */

TEST(DeleteServiceAuth, delete_service_not_auth) {
	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/services/uuidexample";

	auto response = cpr::Delete(cpr::Url{url});
	EXPECT_EQ(response.status_code, 404);
	auto json = nlohmann::json::parse(response.text);
	EXPECT_TRUE(json.contains("error"));
	EXPECT_EQ(json["error"], "not token provided");
}

/*

TEST if user is authenticated but not is admin and not creator
status_code = 401
res.body = 'not enough priviligies';

	// 1. crear un usuario admin
	// 1.1 admin creado, crea un servicio y guardamos el id
	// 2. obtener el codigo de la comunidad
	// 3. crear un usuario neighbor
	// 4. token neighbor
	// 5. haces la peticion de eliminar el primer sericio
  */

/*
	TEST if user is auth, exists 2 community (A y B), the user is admin of A, and services deleted is B community
	so
	status = 403
	res.body = user without admin privileges or not creator of service
 */
class DeleteServiceAdminBAuth : public testing::Test {
	protected:
	std::string _service_id_;
	std::string _admin1_token_;
	std::string _admin_token_;
	std::string _admin_id_;
	std::string _admin1_id_;
	std::string _admin_community_id_;

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

	void register_admin1() {
		std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/auth/register";

		nlohmann::json new_user = {
			{"email", "example1@gmail.com"},
			{"username", "example1"},
			{"password", "P@ssw0rd!"},
			{"type", "admin"},
			{"community_name", "example_community_name1"}};

		auto response = cpr::Post(cpr::Url{url}, cpr::Body{new_user.dump()}, cpr::Header{{"Content-Type", "application/json"}});

		std::string set_cookie_header = response.header["Set-Cookie"];

		size_t token_pos = set_cookie_header.find("token=");
		if (token_pos != std::string::npos) {
			size_t token_start = token_pos + 6;
			size_t token_end = set_cookie_header.find(";", token_start);
			std::string token_value = set_cookie_header.substr(token_start, token_end - token_start);

			_admin1_token_ = token_value;
		} else {
			_admin1_token_ = "";
		}

		auto json = nlohmann::json::parse(response.text);
		_admin1_id_ = json["id"];
	}

	void SetUp() override {
		register_admin();

		std::string url_service = "http://backend:" + std::to_string(HTTP_PORT) + "/api/service";
		nlohmann::json new_user = {
			{"title", "nodeberiaexistir"},
			{"description", "some description 555555555"},
			{"price", 40},
			{"type", "offered"}};
		auto s_create = cpr::Post(cpr::Url{url_service}, cpr::Cookies{{"token", _admin_token_}}, cpr::Body{new_user.dump()}, cpr::Header{{"Content-Type", "application/json"}});
		auto json = nlohmann::json::parse(s_create.text);
		_service_id_ = json["id"];
		register_admin1();
	}

	void TearDown() override {
		clean_community_table();
		clean_user_table();
		// clean_service_table();
	}
};

TEST_F(DeleteServiceAdminBAuth, delete_service_AdminB) {
	std::string url_service = "http://backend:" + std::to_string(HTTP_PORT) + "/api/service/" + _service_id_;
	auto response = cpr::Delete(cpr::Url{url_service}, cpr::Cookies{{"token", _admin1_token_}}, cpr::Header{{"Content-Type", "application/json"}});
	auto json = nlohmann::json::parse(response.text);

	EXPECT_EQ(response.status_code, 400) << "Expected 400 status code for incorrect password: ";
	EXPECT_TRUE(json.contains("error"));
	EXPECT_EQ(json["error"], "incorrect password");
}
/*
   TEST if user is auth, not admin, but the user is creator of service
   status_code = 204
 */

/*

TEST if user is admin and auth but service not found
status_code = 404
res.body = 'service not found'
 */

/*
TEST if user is admin and auth,and service exists
status_code = 204
res.body = ''
 */

/*
TEST if user is admin and auth, but the user isn't the creator
status_code = 204
 */

/*
TEST if user not admin but auth, and the user is the creator
status_code = 204
 */