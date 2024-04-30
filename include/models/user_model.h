#pragma once

#include <utils/errors.h>

#include <format>
#include <iostream>
#include <memory>
#include <pqxx/pqxx>
#include <string>
#include <vector>

class UserModel {
	private:
	std::string _id;
	std::string _community_id;
	std::string _username;
	std::string _email;
	std::string _type;
	int _balance;
	std::string _created_at;
	std::string _updated_at;

	public:
	UserModel();
	UserModel(std::string id, std::string username);

	UserModel(std::string id, std::string community_id, std::string username, std::string email, std::string type, int balance, std::string created_at, std::string updated_at);

	std::string get_id() const;
	std::string get_community_id() const;
	std::string get_username() const;
	std::string get_email() const;
	std::string get_type() const;
	int get_balance() const;
	std::string get_created_at() const;
	std::string get_updated_at() const;

	static std::unique_ptr<UserModel> create_user(pqxx::connection& db, const std::string& community_id, const std::string& username, const std::string& email, const std::string& password, const std::string& type, const int balance, bool throw_when_null = false);
	static std::vector<std::unique_ptr<UserModel>> get_users(pqxx::connection& db);

	static bool exists_id(pqxx::connection& db, const std::string& id);
	static bool exists_username(pqxx::connection& db, const std::string& username);
	static bool exists_email(pqxx::connection& db, const std::string& email);

	static std::unique_ptr<UserModel> get_user_by_id(pqxx::connection& db, const std::string& id, bool throw_when_null = false);
	static std::unique_ptr<UserModel> get_user_by_username(pqxx::connection& db, const std::string& username, bool throw_when_null = false);
	static std::unique_ptr<UserModel> get_user_by_email(pqxx::connection& db, const std::string& email, bool throw_when_null = false);
	static std::string get_password_by_email(pqxx::connection& db, const std::string& email);

	static bool delete_user_by_id(pqxx::connection& db, const std::string& id);
	static bool update_user_by_id(pqxx::connection& db, const std::string& id, const std::string username = "", const std::string email = "", const std::string password = "");
	static bool update_user_admin(pqxx::connection& db, const std::string& id, const std::string username, const int balance);
};
