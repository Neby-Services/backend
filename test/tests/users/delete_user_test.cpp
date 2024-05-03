#include <cpr/cpr.h>
#include <gtest/gtest.h>
#include <unistd.h>
#include <cstdlib> // Para std::getenv
#include <nlohmann/json.hpp>
#include <pqxx/pqxx>
#include <string>
#include <vector>
#include "../common.h"

/*
class deleteSelfNeighbor : public testing::Test
{
protected:
	std::string neighbor_token;
	std::string admin_token;
	std::string admin_id;
	std::string neighbor_id;
	std::string community_id;

	void register_admin()
	{
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
		if (token_pos != std::string::npos)
		{
			size_t token_start = token_pos + 6;
			size_t token_end = set_cookie_header.find(";", token_start);
			std::string token_value = set_cookie_header.substr(token_start, token_end - token_start);

			admin_token = token_value;
		}
		else
		{
			admin_token = "";
		}

		auto json = nlohmann::json::parse(response.text);
		admin_id = json["id"];
	}

	void register_neighbor(std::string code)
	{
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
		if (token_pos != std::string::npos)
		{
			size_t token_start = token_pos + 6;
			size_t token_end = set_cookie_header.find(";", token_start);
			std::string token_value = set_cookie_header.substr(token_start, token_end - token_start);

			neighbor_token = token_value;
		}
		else
		{
			neighbor_token = "";
		}
		// auto json = nlohmann::json::parse(response.text);
		neighbor_id = json["id"];
	}

	void SetUp() override
	{
		register_admin();

		sleep(1);
		pqxx::connection conn(connection_string);

		if (conn.is_open())
		{
			try
			{
				pqxx::work txn(conn);

				pqxx::result result = txn.exec_params("SELECT communities.code FROM users JOIN communities ON users.community_id = communities.id WHERE users.id = $1", admin_id);

				txn.commit();

				community_id = result[0][0].as<std::string>();
			}
			catch (const std::exception &e)
			{
				std::cerr << "Error creating user: " << e.what() << std::endl;
			}
		}
		else
		{
			std::cerr << "Error connecting to the database." << std::endl;
		}
		sleep(1);
		register_neighbor(community_id);
	}

	void TearDown() override
	{
		clean_community_table();
		clean_user_table();
		// clean_service_table();
	}
};
/*

TEST_F(deleteSelfNeighbor, delete_self_bad_request)
{
	std::string url_service = "http://backend:" + std::to_string(HTTP_PORT) + "/api/users/self";
	nlohmann::json req_body = {
	};

	auto response = cpr::Delete(cpr::Url{url_service}, cpr::Body{req_body.dump()}, cpr::Header{{"Content-Type", "application/json"}}, cpr::Cookies{{"token", neighbor_token}});
	EXPECT_EQ(response.status_code, 400);
}


TEST_F(deleteSelfNeighbor, delete_self_Neighbor_user)
{
	std::string url_service = "http://backend:" + std::to_string(HTTP_PORT) + "/api/users/self";
	auto response = cpr::Delete(cpr::Url{url_service}, cpr::Cookies{{"token", admin_token}}, cpr::Header{{"Content-Type", "application/json"}});
	auto json = nlohmann::json::parse(response.text);
	EXPECT_EQ(response.status_code, 200);
	EXPECT_TRUE(json.contains("message"));
	EXPECT_EQ(json["message"], "user deleted successfully");
}

TEST_F(deleteSelfNeighbor, delete_self_admin)
{
	std::string url_service = "http://backend:" + std::to_string(HTTP_PORT) + "/api/users/self";
	auto response = cpr::Delete(cpr::Url{url_service}, cpr::Cookies{{"token", neighbor_token}}, cpr::Header{{"Content-Type", "application/json"}});
	auto json = nlohmann::json::parse(response.text);
	EXPECT_EQ(response.status_code, 403);
	EXPECT_TRUE(json.contains("error"));
	EXPECT_EQ(json["error"], "admin can't delete themselves");
}

*/
class DeleteUserById : public testing::Test
{
protected:
	std::string neighbor_token;
	std::string admin_token;
	std::string admin_id;
	std::string neighbor_id;
	std::string community_id;

	void register_admin()
	{
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
		if (token_pos != std::string::npos)
		{
			size_t token_start = token_pos + 6;
			size_t token_end = set_cookie_header.find(";", token_start);
			std::string token_value = set_cookie_header.substr(token_start, token_end - token_start);

			admin_token = token_value;
		}
		else
		{
			admin_token = "";
		}

		auto json = nlohmann::json::parse(response.text);
		admin_id = json["id"];
	}

	void register_neighbor(std::string code)
	{
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
		if (token_pos != std::string::npos)
		{
			size_t token_start = token_pos + 6;
			size_t token_end = set_cookie_header.find(";", token_start);
			std::string token_value = set_cookie_header.substr(token_start, token_end - token_start);

			neighbor_token = token_value;
		}
		else
		{
			neighbor_token = "";
		}
		// auto json = nlohmann::json::parse(response.text);
		neighbor_id = json["id"];
	}

	void SetUp() override
	{
		register_admin();

		sleep(1);
		pqxx::connection conn(connection_string);

		if (conn.is_open())
		{
			try
			{
				pqxx::work txn(conn);

				pqxx::result result = txn.exec_params("SELECT communities.code FROM users JOIN communities ON users.community_id = communities.id WHERE users.id = $1", admin_id);

				txn.commit();

				community_id = result[0][0].as<std::string>();
			}
			catch (const std::exception &e)
			{
				std::cerr << "Error creating user: " << e.what() << std::endl;
			}
		}
		else
		{
			std::cerr << "Error connecting to the database." << std::endl;
		}
		sleep(1);
		register_neighbor(community_id);
	}

	void TearDown() override
	{
		clean_community_table();
		clean_user_table();
		// clean_service_table();
	}
};


TEST_F(DeleteUserById, delete_by_id_admin_succes) {
	sleep(1);
	std::string url_service = "http://backend:" + std::to_string(HTTP_PORT) + "/api/users/" + neighbor_id;
	
	auto response = cpr::Delete(cpr::Url{url_service}, cpr::Cookies{{"token", admin_token}}, cpr::Header{{"Content-Type", "application/json"}});
	auto json = nlohmann::json::parse(response.text);
	EXPECT_EQ(response.status_code, 200) << "Expected 200 status code for user deleted successfully: ";
	EXPECT_TRUE(json.contains("message"));
	EXPECT_EQ(json["message"], "user deleted successfully");
}

TEST_F(DeleteUserById, delete_by_id_neighbor_fail) {
	sleep(1);
	std::string url_service = "http://backend:" + std::to_string(HTTP_PORT) + "/api/users/" + admin_token;
	
	auto response = cpr::Delete(cpr::Url{url_service}, cpr::Cookies{{"token", neighbor_token}}, cpr::Header{{"Content-Type", "application/json"}});
	auto json = nlohmann::json::parse(response.text);
	EXPECT_EQ(response.status_code, 403) << "Expected 403 status code for not enough privileges: ";
	EXPECT_TRUE(json.contains("error"));
	EXPECT_EQ(json["error"], "not enough privileges");
}


TEST_F(DeleteUserById, delete_by_id_admin_invalid_id) {
	sleep(1);
	std::string url_service = "http://backend:" + std::to_string(HTTP_PORT) + "/api/users/6fe43f0-8b97";
	
	auto response = cpr::Delete(cpr::Url{url_service}, cpr::Cookies{{"token", admin_token}}, cpr::Header{{"Content-Type", "application/json"}});
	auto json = nlohmann::json::parse(response.text);
	EXPECT_EQ(response.status_code, 400) << "Expected 400 status code for invalid id: ";
	EXPECT_TRUE(json.contains("error"));
	EXPECT_EQ(json["error"], "invalid id");
}


TEST_F(DeleteUserById, delete_by_id_admin_not_found) {
	sleep(1);
	std::string url_service = "http://backend:" + std::to_string(HTTP_PORT) + "/api/users/76fe43f0-8b97-4079-94e8-ef6f10d759b0";
	
	auto response = cpr::Delete(cpr::Url{url_service}, cpr::Cookies{{"token", admin_token}}, cpr::Header{{"Content-Type", "application/json"}});
	auto json = nlohmann::json::parse(response.text);
	EXPECT_EQ(response.status_code, 404) << "Expected 403 status code for invalid id: ";
	EXPECT_TRUE(json.contains("error"));
	EXPECT_EQ(json["error"], "user not found");
}


class DeleteUserByIdDiffCommunity : public testing::Test
{
protected:
	std::string neighbor_token;
	std::string admin_token;
	std::string admin_token2;
	std::string admin_id2;
	std::string admin_id;
	std::string neighbor_id;
	std::string community_id;

	void register_admin()
	{
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
		if (token_pos != std::string::npos)
		{
			size_t token_start = token_pos + 6;
			size_t token_end = set_cookie_header.find(";", token_start);
			std::string token_value = set_cookie_header.substr(token_start, token_end - token_start);

			admin_token = token_value;
		}
		else
		{
			admin_token = "";
		}

		auto json = nlohmann::json::parse(response.text);
		admin_id = json["id"];
	}

	void register_neighbor(std::string code)
	{
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
		if (token_pos != std::string::npos)
		{
			size_t token_start = token_pos + 6;
			size_t token_end = set_cookie_header.find(";", token_start);
			std::string token_value = set_cookie_header.substr(token_start, token_end - token_start);

			neighbor_token = token_value;
		}
		else
		{
			neighbor_token = "";

		}
		// auto json = nlohmann::json::parse(response.text);
		neighbor_id = json["id"];
	}

	void register_admin2()
	{
		std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/auth/register";

		nlohmann::json new_user = {
			{"email", "exampleee@gmail.com"},
			{"username", "exampleee"},
			{"password", "P@ssw0rd!"},
			{"type", "admin"},
			{"community_name", "second_example_community_name"}};

		auto response = cpr::Post(cpr::Url{url}, cpr::Body{new_user.dump()}, cpr::Header{{"Content-Type", "application/json"}});

		std::string set_cookie_header = response.header["Set-Cookie"];

		size_t token_pos = set_cookie_header.find("token=");
		if (token_pos != std::string::npos)
		{
			size_t token_start = token_pos + 6;
			size_t token_end = set_cookie_header.find(";", token_start);
			std::string token_value = set_cookie_header.substr(token_start, token_end - token_start);

			admin_token2 = token_value;
		}
		else
		{
			admin_token2 = "";
		}

		auto json = nlohmann::json::parse(response.text);
		admin_id2 = json["id"];
	}

	void SetUp() override
	{
		register_admin();

		sleep(1);
		pqxx::connection conn(connection_string);

		if (conn.is_open())
		{
			try
			{
				pqxx::work txn(conn);

				pqxx::result result = txn.exec_params("SELECT communities.code FROM users JOIN communities ON users.community_id = communities.id WHERE users.id = $1", admin_id);

				txn.commit();

				community_id = result[0][0].as<std::string>();
			}
			catch (const std::exception &e)
			{
				std::cerr << "Error creating user: " << e.what() << std::endl;
			}
		}
		else
		{
			std::cerr << "Error connecting to the database." << std::endl;
		}
		sleep(1);
		register_neighbor(community_id);
		register_admin2();
	}

	void TearDown() override
	{
		clean_community_table();
		clean_user_table();
		// clean_service_table();
	}
};

TEST_F(DeleteUserByIdDiffCommunity, delete_by_id_admin_diff_community) {
	sleep(1);
	std::string url_service = "http://backend:" + std::to_string(HTTP_PORT) + "/api/users/" + neighbor_id;;
	
	auto response = cpr::Delete(cpr::Url{url_service}, cpr::Cookies{{"token", admin_token2}}, cpr::Header{{"Content-Type", "application/json"}});
	auto json = nlohmann::json::parse(response.text);
	EXPECT_EQ(response.status_code, 403) << "Expected 403 status code for invalid id: ";
	EXPECT_TRUE(json.contains("error"));
	EXPECT_EQ(json["error"], "not enough privileges");
}

