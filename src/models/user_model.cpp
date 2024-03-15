#include "models/user_model.h"

#include <iostream>

UserModel::UserModel(std::string id) : _id(id) {}

UserModel::UserModel(std::string id, std::string email, std::string username, std::string image_url, int balance, std::string type) : _id(id), _email(email), _username(username), _image_url(image_url), _balance(balance), _type(type) {}

std::string UserModel::getId() {
	return _id;
}

std::string UserModel::getEmail() {
	return _email;
}

std::string UserModel::getUsername() {
	return _username;
}

std::string UserModel::getImageUrl() {
	return _image_url;
}

int UserModel::getBalance() {
	return _balance;
}

std::string UserModel::getType() {
	return _type;
}

UserModel UserModel::create_user(pqxx::connection& db, std::string password, std::string email, std::string username, std::string image_url, int balance, std::string type) {
	pqxx::work txn(db);

	txn.exec_params("INSERT INTO users (password, email, username, image_url, balance, type) VALUES ($1, $2, $3, $4, $5, $6)", password, email, username, image_url, balance, type);

	pqxx::result result = txn.exec_params("SELECT id FROM users WHERE email = $1", email);

	std::string res = result[0][0].as<std::string>();
	txn.commit();
	return UserModel(res);
}

bool UserModel::user_exist(pqxx::connection& db, std::string email, std::string username) {
	try {
		pqxx::work txn(db);

		pqxx::result result = txn.exec_params("SELECT username FROM users WHERE email = $1 OR username = $2", email, username);

		bool userExists = !result.empty() && !result[0][0].is_null();

		txn.commit();

		return userExists;
	} catch (const std::exception& e) {
		return false;
	}
}

std::vector<UserModel> UserModel::get_users(pqxx::connection& db) {
	std::vector<UserModel> all_users;

	pqxx::work txn(db);

	pqxx::result result = txn.exec("SELECT id, email, username, image_url, balance, type FROM users");

	for (const auto& row : result) {
		UserModel user(row["id"].as<std::string>(), row["email"].as<std::string>(), row["username"].as<std::string>(), row["image_url"].as<std::string>(), row["balance"].as<int>(), row["type"].as<std::string>());

		all_users.push_back(user);
	}

	txn.commit();

	return all_users;
}
