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
TEST(UpdateAdminAuth, update_admin_not_auth) {
	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/users/123";

	auto response = cpr::Delete(cpr::Url{url});
	EXPECT_EQ(response.status_code, 404);
	auto json = nlohmann::json::parse(response.text);
	EXPECT_TRUE(json.contains("error"));
	EXPECT_EQ(json["error"], "not token provided");
}

/*

TEST if user is authenticated but is not admin
status_code = 401
res.body = 'not enough priviligies';

*/
class UpdateAdminNotAdmin : public testing::Test {
	protected:
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
	}

	void TearDown() override {
		clean_community_table();
		clean_user_table();
		// clean_service_table();
	}
};

TEST_F(UpdateAdminNotAdmin, update_admin_not_admin) {
	sleep(1);
	std::string url_service = "http://backend:" + std::to_string(HTTP_PORT) + "/api/users/" + _admin_id_;
	auto response = cpr::Put(cpr::Url{url_service}, cpr::Cookies{{"token", _neighbor_token_}}, cpr::Header{{"Content-Type", "application/json"}});
	auto json = nlohmann::json::parse(response.text);
	EXPECT_EQ(response.status_code, 403) << "Expected 403 status code for not enough privileges: ";
	EXPECT_TRUE(json.contains("error"));
	EXPECT_EQ(json["error"], "not enough privileges");
}

/*

TEST if user is authenticated but and admin but invalid Id (doesnt follow the psql UUID format)
status_code = 400
res.body = 'invalid id';

*/

class UpdateAdminInvalidId : public testing::Test {
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

TEST_F(UpdateAdminInvalidId, update_admin_invalid_id) {
	sleep(1);
	std::string url_service = "http://backend:" + std::to_string(HTTP_PORT) + "/api/users/" + "123";
	auto response = cpr::Put(cpr::Url{url_service}, cpr::Cookies{{"token", _admin_token_}}, cpr::Header{{"Content-Type", "application/json"}});
	auto json = nlohmann::json::parse(response.text);
	EXPECT_EQ(response.status_code, 400) << "Expected 400 status code for invalid id: ";
	EXPECT_TRUE(json.contains("error"));
	EXPECT_EQ(json["error"], "invalid id");
}

/*

TEST if user is authenticated and admin but neighbour is not found
status_code = 404
res.body = 'user not found';

*/

class UpdateAdminNotFound : public testing::Test {
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

TEST_F(UpdateAdminNotFound, update_admin_not_found) {
	sleep(1);
	std::string url_service = "http://backend:" + std::to_string(HTTP_PORT) + "/api/users/" + "d39f6c52-5de7-44b6-9ea4-87ccb1055097";
	auto response = cpr::Put(cpr::Url{url_service}, cpr::Cookies{{"token", _admin_token_}}, cpr::Header{{"Content-Type", "application/json"}});
	auto json = nlohmann::json::parse(response.text);
	EXPECT_EQ(response.status_code, 404) << "Expected 404 status code for uer not found: ";
	EXPECT_TRUE(json.contains("error"));
	EXPECT_EQ(json["error"], "user not found");
}

/*

TEST if user is authenticated and admin but from a diferent community
status_code = 403
res.body = 'not enough privileges';

*/

class UpdateAdminCommunityB : public testing::Test {
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
		sleep(1);
		register_admin1();
	}

	void TearDown() override {
		clean_community_table();
		clean_user_table();
		// clean_service_table();
	}
};

/*

TEST if user is authenticated and admin but new username or balance is invalid
status_code = 400
res.body = 'invalid username' / 'invlaid balance';

*/

class UpdateAdminInvalidUsername : public testing::Test {
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
		// clean_service_table();
	}
};

TEST_F(UpdateAdminInvalidUsername, update_admin_invalid_username) {
	sleep(1);
	std::string url_service = "http://backend:" + std::to_string(HTTP_PORT) + "/api/users/" + _admin_id_;
	nlohmann::json update = {
		{"username", "1"}};
	auto response = cpr::Put(cpr::Url{url_service}, cpr::Body{update.dump()}, cpr::Cookies{{"token", _admin_token_}}, cpr::Header{{"Content-Type", "application/json"}});
	auto json = nlohmann::json::parse(response.text);
	EXPECT_EQ(response.status_code, 400) << "Expected 400 status code for invalid username: ";
	EXPECT_TRUE(json.contains("error"));
	EXPECT_EQ(json["error"], "incorrect username");
}

TEST_F(UpdateAdminInvalidUsername, update_admin_invalid_balance) {
	sleep(1);
	std::string url_service = "http://backend:" + std::to_string(HTTP_PORT) + "/api/users/" + _admin_id_;
	nlohmann::json update = {
		{"balance", "-1"}};
	auto response = cpr::Put(cpr::Url{url_service}, cpr::Body{update.dump()}, cpr::Cookies{{"token", _admin_token_}}, cpr::Header{{"Content-Type", "application/json"}});
	auto json = nlohmann::json::parse(response.text);
	EXPECT_EQ(response.status_code, 400) << "Expected 400 status code for invalid balance: ";
	EXPECT_TRUE(json.contains("error"));
	EXPECT_EQ(json["error"], "invalid balance");
}

TEST_F(UpdateAdminInvalidUsername, update_admin_succes) {
	sleep(1);
	std::string url_service = "http://backend:" + std::to_string(HTTP_PORT) + "/api/users/" + _admin_id_;
	nlohmann::json update = {
		{"balance", "1"},
		{"username", "example"}};
	auto response = cpr::Put(cpr::Url{url_service}, cpr::Body{update.dump()}, cpr::Cookies{{"token", _admin_token_}}, cpr::Header{{"Content-Type", "application/json"}});
	auto json = nlohmann::json::parse(response.text);
	EXPECT_EQ(response.status_code, 200) << "Expected 200 status code for user updated succesfully: ";
	EXPECT_TRUE(json.contains("message"));
	EXPECT_EQ(json["message"], "User updated successfully");
}