#include <cpr/cpr.h>
#include <gtest/gtest.h>
#include <unistd.h>
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
TEST(CreateRatingAuth, create_rating_not_auth) {
	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/ratings/2";

	auto response = cpr::Post(cpr::Url{url});
	EXPECT_EQ(response.status_code, 404);
	auto json = nlohmann::json::parse(response.text);
	EXPECT_TRUE(json.contains("error"));
	EXPECT_EQ(json["error"], "no token provided");
}

/*

TEST if user is authenticated but incorrect UUID format
status_code = 400
res.body = 'invalid UUID format';

*/
class CreateRatingInvalidUUID : public testing::Test {
    protected:
    std::string _admin_token_;
    std::string _admin_id_;

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
    }

    void TearDown() override {
        clean_community_table();
        clean_user_table();
    }
};

TEST_F(CreateRatingInvalidUUID, create_rating_invalid_UUID) {
    std::string url_service = "http://backend:" + std::to_string(HTTP_PORT) + "/api/ratings/vesasaber"  ;
	auto response = cpr::Post(cpr::Url{url_service}, cpr::Cookies{{"token", _admin_token_}}, cpr::Header{{"Content-Type", "application/json"}});
	auto json = nlohmann::json::parse(response.text);

	EXPECT_EQ(response.status_code, 400) << "Expected 400 status code for invalid UUID format: ";
	EXPECT_TRUE(json.contains("error"));
	EXPECT_EQ(json["error"], "invalid UUID format");
}

TEST_F(CreateRatingInvalidUUID, create_rating_no_body) {
    std::string url_service = "http://backend:" + std::to_string(HTTP_PORT) + "/api/ratings/75b7a53a-e207-4d68-8d57-57a6c9393732"  ;
        
	auto response = cpr::Post(cpr::Url{url_service}, cpr::Cookies{{"token", _admin_token_}}, cpr::Header{{"Content-Type", "application/json"}});
	auto json = nlohmann::json::parse(response.text);

	EXPECT_EQ(response.status_code, 404) << "Expected 404 status code for missing rating field: ";
	EXPECT_TRUE(json.contains("error"));
	EXPECT_EQ(json["error"], "missing rating field");
}

TEST_F(CreateRatingInvalidUUID, create_rating_service_not_found) {

	std::string create_service_url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/services";
    nlohmann::json new_service = {
        {"title", "nodeberiaexistir"},
        {"description", "some description 555555555"},
        {"price", 40},
        {"type", "offered"} };
    auto s_create = cpr::Post(cpr::Url{ create_service_url }, cpr::Cookies{ {"token", _admin_token_} }, cpr::Body{ new_service.dump() }, cpr::Header{ {"Content-Type", "application/json"} });
    auto s_json = nlohmann::json::parse(s_create.text);
    std::string _service_id_ = s_json["id"];


    std::string url_service = "http://backend:" + std::to_string(HTTP_PORT) + "/api/ratings/75b7a53a-e207-4d68-8d57-57a6c9393732"  ;

    nlohmann::json new_rating = {
        {"rating", "9"} };

	auto response = cpr::Post(cpr::Url{url_service}, cpr::Cookies{{"token", _admin_token_}}, cpr::Body{ new_rating.dump() }, cpr::Header{{"Content-Type", "application/json"}});
	auto json = nlohmann::json::parse(response.text);

	EXPECT_EQ(response.status_code, 404) << "Expected 404 status code for service not found: ";
	EXPECT_TRUE(json.contains("error"));
	EXPECT_EQ(json["error"], "service not found");
}

TEST_F(CreateRatingInvalidUUID, create_rating_service_not_closed) {
    std::string create_service_url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/services";
    nlohmann::json new_service = {
        {"title", "nodeberiaexistir"},
        {"description", "some description 555555555"},
        {"price", 40},
        {"type", "offered"} };
    auto s_create = cpr::Post(cpr::Url{ create_service_url }, cpr::Cookies{ {"token", _admin_token_} }, cpr::Body{ new_service.dump() }, cpr::Header{ {"Content-Type", "application/json"} });
    auto s_json = nlohmann::json::parse(s_create.text);
    std::string _service_id_ = s_json["id"];


    std::string url_service = "http://backend:" + std::to_string(HTTP_PORT) + "/api/ratings/" + _service_id_  ;

    nlohmann::json new_rating = {
        {"rating", "9"} };

	auto response = cpr::Post(cpr::Url{url_service}, cpr::Cookies{{"token", _admin_token_}}, cpr::Body{ new_rating.dump() }, cpr::Header{{"Content-Type", "application/json"}});
	auto json = nlohmann::json::parse(response.text);

	EXPECT_EQ(response.status_code, 400) << "Expected 400 status code for service not closed: ";
	EXPECT_TRUE(json.contains("error"));
	EXPECT_EQ(json["error"], "service not closed");
}

class CreateRatingCorrect : public testing::Test {
    protected:
    std::string _service_id_;
	std::string _neighbor_token_;
	std::string _admin_token_;
	std::string _admin_id_;
	std::string _neighbor_id_;
	std::string _community_id_;

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

	void register_neighbor(std::string code) {
		std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/auth/register";
		nlohmann::json new_user = {
			{"email", "examplee@gmail.com"},
			{"username", "examplee"},
			{"password", "P@ssw0rd!"},
			{"type", "neighbor"},
			{"community_code", code} };

		auto response = cpr::Post(cpr::Url{ url }, cpr::Body{ new_user.dump() }, cpr::Header{ {"Content-Type", "application/json"} });
		auto json = nlohmann::json::parse(response.text);
		std::string set_cookie_header = response.header["Set-Cookie"];

		size_t token_pos = set_cookie_header.find("token=");
		if (token_pos != std::string::npos) {
			size_t token_start = token_pos + 6;
			size_t token_end = set_cookie_header.find(";", token_start);
			std::string token_value = set_cookie_header.substr(token_start, token_end - token_start);

			_neighbor_token_ = token_value;
		}
		else {
			_neighbor_token_ = "";
		}
		// auto json = nlohmann::json::parse(response.text);
		_neighbor_id_ = json["id"];
	}

    void SetUp() override {
        register_admin();

				pqxx::connection conn(connection_string);

		if (conn.is_open()) {
			try {
				pqxx::work txn(conn);

				pqxx::result result = txn.exec_params("SELECT communities.code FROM users JOIN communities ON users.community_id = communities.id WHERE users.id = $1", _admin_id_);

				txn.commit();

				_community_id_ = result[0][0].as<std::string>();
			}
			catch (const std::exception& e) {
				std::cerr << "Error creating user: " << e.what() << std::endl;
			}
		}
		else {
			std::cerr << "Error connecting to the database." << std::endl;
		}

		register_neighbor(_community_id_);

		std::string create_service_url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/services";
    	nlohmann::json new_service = {
    	    {"title", "nodeberiaexistir"},
    	    {"description", "some description 555555555"},
    	    {"price", 40},
    	    {"type", "requested"} };
    	auto s_create = cpr::Post(cpr::Url{ create_service_url }, cpr::Cookies{ {"token", _admin_token_} }, cpr::Body{ new_service.dump() }, cpr::Header{ {"Content-Type", "application/json"} });
    	auto s_json = nlohmann::json::parse(s_create.text);
    	_service_id_ = s_json["id"];
	}

	void TearDown() override {
		clean_community_table();
		clean_user_table();
	}
};

TEST_F(CreateRatingCorrect, create_rating_service_correct) {

	std::string create_notification_url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/notifications?type=services&service_id=" + _service_id_;
	auto n_create = cpr::Post(cpr::Url{ create_notification_url }, cpr::Cookies{ {"token", _neighbor_token_} }, cpr::Header{ {"Content-Type", "application/json"} });
	auto n_json = nlohmann::json::parse(n_create.text);
	std::string _notification_id_ = n_json["notification_service"]["id"];

	std::string n_accept_url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/notifications/" + _notification_id_ + "?action=accepted" ;
	auto n_accept = cpr::Put(cpr::Url{ n_accept_url }, cpr::Cookies{ {"token", _admin_token_} }, cpr::Header{ {"Content-Type", "application/json"} });
	
    std::string url_rating = "http://backend:" + std::to_string(HTTP_PORT) + "/api/ratings/" + _service_id_  ;
    nlohmann::json new_rating = {
        {"rating", "9"} };
	auto response = cpr::Post(cpr::Url{url_rating}, cpr::Cookies{{"token", _admin_token_}}, cpr::Body{ new_rating.dump() }, cpr::Header{{"Content-Type", "application/json"}});
	auto json = nlohmann::json::parse(response.text);

	EXPECT_EQ(response.status_code, 201) << "Expected 201 status code for rating ccreated succesfully: ";
	EXPECT_TRUE(json.contains("id"));
}

/*

TEST if user is authenticated but service does not exist
status_code = 404
res.body = 'service not found';

*/