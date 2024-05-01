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
status_code = 403
res.body = 'not enough priviligies';

*/
class DeleteServiceNeitherAuth : public testing::Test {
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
			{"community_code", code}};

		auto response = cpr::Post(cpr::Url{url}, cpr::Body{new_user.dump()}, cpr::Header{{"Content-Type", "application/json"}});
		auto json = nlohmann::json::parse(response.text);
		std::string set_cookie_header = response.header["Set-Cookie"];

		size_t token_pos = set_cookie_header.find("token=");
		if (token_pos != std::string::npos) {
			size_t token_start = token_pos + 6;
			size_t token_end = set_cookie_header.find(";", token_start);
			std::string token_value = set_cookie_header.substr(token_start, token_end - token_start);

			_neighbor_token_ = token_value;
		} else {
			_neighbor_token_ = "";
		}
		// auto json = nlohmann::json::parse(response.text);
		_neighbor_id_ = json["id"];
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
		pqxx::connection conn(connection_string);

		if (conn.is_open()) {
			try {
				pqxx::work txn(conn);

				pqxx::result result = txn.exec_params("SELECT communities.code FROM users JOIN communities ON users.community_id = communities.id WHERE users.id = $1", _admin_id_);

				txn.commit();

				_community_id_ = result[0][0].as<std::string>();
			} catch (const std::exception& e) {
				std::cerr << "Error creating user: " << e.what() << std::endl;
			}
		} else {
			std::cerr << "Error connecting to the database." << std::endl;
		}
		sleep(1);
		register_neighbor(_community_id_);
	}

	void TearDown() override {
		clean_community_table();
		clean_user_table();
		// clean_service_table();
	}
};

TEST_F(DeleteServiceNeitherAuth, delete_service_Neither) {
	std::string url_service = "http://backend:" + std::to_string(HTTP_PORT) + "/api/services/" + _service_id_;
	auto response = cpr::Delete(cpr::Url{url_service}, cpr::Cookies{{"token", _neighbor_token_}}, cpr::Header{{"Content-Type", "application/json"}});
	auto json = nlohmann::json::parse(response.text);

	EXPECT_EQ(response.status_code, 403) << "Expected 403 status code for user without admin privileges or not creator of service: ";
	EXPECT_TRUE(json.contains("error"));
	EXPECT_EQ(json["error"], "user without admin privileges or not creator of service");
}

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
			{"email", "examplee@gmail.com"},
			{"username", "examplee"},
			{"password", "P@ssw0rd!"},
			{"type", "admin"},
			{"community_name", "example_community_namee"}};

		auto response = cpr::Post(cpr::Url{url}, cpr::Body{new_user.dump()}, cpr::Header{{"Content-Type", "application/json"}});
		auto json = nlohmann::json::parse(response.text);
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
		// auto json = nlohmann::json::parse(response.text);
		_admin1_id_ = json["id"];
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
		register_admin1();
	}

	void TearDown() override {
		clean_community_table();
		clean_user_table();
		// clean_service_table();
	}
};

TEST_F(DeleteServiceAdminBAuth, delete_service_AdminB) {
	std::string url_service = "http://backend:" + std::to_string(HTTP_PORT) + "/api/services/" + _service_id_;
	auto response = cpr::Delete(cpr::Url{url_service}, cpr::Cookies{{"token", _admin1_token_}}, cpr::Header{{"Content-Type", "application/json"}});
	auto json = nlohmann::json::parse(response.text);

	EXPECT_EQ(response.status_code, 403) << "Expected 403 status code for user without admin privileges or not creator of service: ";
	EXPECT_TRUE(json.contains("error"));
	EXPECT_EQ(json["error"], "user without admin privileges or not creator of service");
}
/*
TEST if user is auth, not admin, but the user is creator of service
status_code = 204
*/
class DeleteServiceCreatorAuth : public testing::Test {
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
			{"community_code", code}};

		auto response = cpr::Post(cpr::Url{url}, cpr::Body{new_user.dump()}, cpr::Header{{"Content-Type", "application/json"}});
		auto json = nlohmann::json::parse(response.text);
		std::string set_cookie_header = response.header["Set-Cookie"];

		size_t token_pos = set_cookie_header.find("token=");
		if (token_pos != std::string::npos) {
			size_t token_start = token_pos + 6;
			size_t token_end = set_cookie_header.find(";", token_start);
			std::string token_value = set_cookie_header.substr(token_start, token_end - token_start);

			_neighbor_token_ = token_value;
		} else {
			_neighbor_token_ = "";
		}
		// auto json = nlohmann::json::parse(response.text);
		_neighbor_id_ = json["id"];
	}

	void SetUp() override {
		register_admin();

		sleep(1);
		pqxx::connection conn(connection_string);

		if (conn.is_open()) {
			try {
				pqxx::work txn(conn);

				pqxx::result result = txn.exec_params("SELECT communities.code FROM users JOIN communities ON users.community_id = communities.id WHERE users.id = $1", _admin_id_);

				txn.commit();

				_community_id_ = result[0][0].as<std::string>();
			} catch (const std::exception& e) {
				std::cerr << "Error creating user: " << e.what() << std::endl;
			}
		} else {
			std::cerr << "Error connecting to the database." << std::endl;
		}
		sleep(1);
		register_neighbor(_community_id_);
		std::string url_service = "http://backend:" + std::to_string(HTTP_PORT) + "/api/services";
		nlohmann::json new_service = {
			{"title", "nodeberiaexistir"},
			{"description", "some description 555555555"},
			{"price", 40},
			{"type", "offered"}};
		auto s_create = cpr::Post(cpr::Url{url_service}, cpr::Cookies{{"token", _neighbor_token_}}, cpr::Body{new_service.dump()}, cpr::Header{{"Content-Type", "application/json"}});
		auto json = nlohmann::json::parse(s_create.text);
		_service_id_ = json["id"];
	}

	void TearDown() override {
		clean_community_table();
		clean_user_table();
		// clean_service_table();
	}
};

TEST_F(DeleteServiceCreatorAuth, delete_service_creator) {
	sleep(1);
	std::string url_service = "http://backend:" + std::to_string(HTTP_PORT) + "/api/services/" + _service_id_;
	auto response = cpr::Delete(cpr::Url{url_service}, cpr::Cookies{{"token", _neighbor_token_}}, cpr::Header{{"Content-Type", "application/json"}});
	auto json = nlohmann::json::parse(response.text);
	EXPECT_EQ(response.status_code, 200) << "Expected 200 status code for service deleted succesfully: ";
	EXPECT_TRUE(json.contains("message"));
	EXPECT_EQ(json["message"], "service deleted succesfully");
}
/*

TEST if user is admin and auth but service not found
status_code = 404
res.body = 'service not found'
*/
class DeleteServiceNotFoundAuth : public testing::Test {
	protected:
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
	}

	void SetUp() override {
		register_admin();
	}

	void TearDown() override {
		clean_community_table();
		clean_user_table();
		// clean_service_table();
	}
};

TEST_F(DeleteServiceNotFoundAuth, delete_service_not_found) {
	std::string lol = "123e4567-e89b-12d3-a456-426655440000";
	std::string url_service = "http://backend:" + std::to_string(HTTP_PORT) + "/api/services/" + lol;
	auto response = cpr::Delete(cpr::Url{url_service}, cpr::Cookies{{"token", _admin_token_}}, cpr::Header{{"Content-Type", "application/json"}});
	auto json = nlohmann::json::parse(response.text);

	EXPECT_EQ(response.status_code, 404) << "Expected 404 status code for service not found: ";
	EXPECT_TRUE(json.contains("error"));
	EXPECT_EQ(json["error"], "service not found");
}
/*
TEST if user is admin and auth,and service exists
status_code = 204
res.body = ''
*/
class DeleteServiceAdminAuth : public testing::Test {
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
			{"community_code", code}};

		auto response = cpr::Post(cpr::Url{url}, cpr::Body{new_user.dump()}, cpr::Header{{"Content-Type", "application/json"}});
		auto json = nlohmann::json::parse(response.text);
		std::string set_cookie_header = response.header["Set-Cookie"];

		size_t token_pos = set_cookie_header.find("token=");
		if (token_pos != std::string::npos) {
			size_t token_start = token_pos + 6;
			size_t token_end = set_cookie_header.find(";", token_start);
			std::string token_value = set_cookie_header.substr(token_start, token_end - token_start);

			_neighbor_token_ = token_value;
		} else {
			_neighbor_token_ = "";
		}
		// auto json = nlohmann::json::parse(response.text);
		_neighbor_id_ = json["id"];
	}

	void SetUp() override {
		register_admin();

		sleep(1);
		pqxx::connection conn(connection_string);

		if (conn.is_open()) {
			try {
				pqxx::work txn(conn);

				pqxx::result result = txn.exec_params("SELECT communities.code FROM users JOIN communities ON users.community_id = communities.id WHERE users.id = $1", _admin_id_);

				txn.commit();

				_community_id_ = result[0][0].as<std::string>();
			} catch (const std::exception& e) {
				std::cerr << "Error creating user: " << e.what() << std::endl;
			}
		} else {
			std::cerr << "Error connecting to the database." << std::endl;
		}
		sleep(1);
		register_neighbor(_community_id_);
		std::string url_service = "http://backend:" + std::to_string(HTTP_PORT) + "/api/services";
		nlohmann::json new_service = {
			{"title", "nodeberiaexistir"},
			{"description", "some description 555555555"},
			{"price", 40},
			{"type", "offered"}};
		auto s_create = cpr::Post(cpr::Url{url_service}, cpr::Cookies{{"token", _neighbor_token_}}, cpr::Body{new_service.dump()}, cpr::Header{{"Content-Type", "application/json"}});
		auto json = nlohmann::json::parse(s_create.text);
		_service_id_ = json["id"];
	}

	void TearDown() override {
		clean_community_table();
		clean_user_table();
		// clean_service_table();
	}
};

TEST_F(DeleteServiceAdminAuth, delete_service_admin) {
	sleep(1);
	std::string url_service = "http://backend:" + std::to_string(HTTP_PORT) + "/api/services/" + _service_id_;
	auto response = cpr::Delete(cpr::Url{url_service}, cpr::Cookies{{"token", _admin_token_}}, cpr::Header{{"Content-Type", "application/json"}});
	auto json = nlohmann::json::parse(response.text);
	EXPECT_EQ(response.status_code, 200) << "Expected 200 status code for service deleted succesfully: ";
	EXPECT_TRUE(json.contains("message"));
	EXPECT_EQ(json["message"], "service deleted succesfully");
}
