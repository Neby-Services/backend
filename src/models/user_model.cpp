#include "models/user_model.h"

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
