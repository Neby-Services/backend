#include "models/user_model.h"

#include <iostream>

UserModel::UserModel(std::string id) : _id(id) {}

std::string UserModel::getId() {
	return _id;
}

UserModel UserModel::create_user(pqxx::connection& db, std::string password, std::string email, std::string username, std::string image_url, int balance, std::string type) {
	pqxx::work txn(db);

	txn.exec_params("INSERT INTO users (password, email, username, image_url, balance, type) VALUES ($1, $2, $3, $4, $5, $6)", password, email, username, image_url, balance, type);

	pqxx::result result = txn.exec_params("SELECT id FROM users WHERE email = $1", email);

	std::string res = result[0][0].as<std::string>();
	txn.commit();
	return UserModel(res);
}

bool UserModel::user_exist(pqxx::connection& db, std::string email) {
	try {
		pqxx::work txn(db);

		pqxx::result result = txn.exec_params("SELECT username FROM users WHERE email = $1", email);

		bool userExists = !result.empty() && !result[0][0].is_null();

		txn.commit();

		return userExists;
	} catch (const std::exception& e) {
		return false;
	}
}

bool UserModel::username_exist(pqxx::connection& db, std::string username) {
	try {
		pqxx::work txn(db);

		pqxx::result result = txn.exec_params("SELECT email FROM users WHERE username = $1", username);

		bool usernameExists = !result.empty() && !result[0][0].is_null();

		txn.commit();

		return usernameExists;
	} catch (const std::exception& e) {
		return false;
	}
}
