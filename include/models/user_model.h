#pragma once

#include <memory>
#include <pqxx/pqxx>
#include <string>
#include <vector>

class UserModel {
	private:
	std::string _id;
	std::string _email;
	std::string _username;
	std::string _image_url;
	int _balance;
	std::string _type;

	public:
	UserModel(std::string id);
	UserModel(std::string id, std::string email, std::string username, std::string image_url, int balance, std::string type);

	std::string getId();
	std::string getEmail();
	std::string getUsername();
	std::string getImageUrl();
	int getBalance();
	std::string getType();

	static UserModel create_user(pqxx::connection& db, std::string password, std::string email, std::string username, std::string image_url, int balance, std::string type);
	static bool user_exist(pqxx::connection& db, std::string email, std::string ussername);
	static std::vector<UserModel> get_users(pqxx::connection& db);
	static void set_community_id(pqxx::connection& db, std::string community_id, std::string user_id);

	static std::unique_ptr<UserModel> get_user_by_email(pqxx::connection& db, std::string email);
	static std::string get_password_by_email(pqxx::connection& db, std::string email);
};
