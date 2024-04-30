#include <models/user_model.h>

UserModel::UserModel() : _id("") {}
UserModel::UserModel(std::string id, std::string username) : _id(id), _username(username) {}

UserModel::UserModel(std::string id, std::string community_id, std::string username, std::string email, std::string type, int balance, std::string created_at, std::string updated_at) : _id(id), _community_id(community_id), _username(username), _email(email), _type(type), _balance(balance), _created_at(created_at), _updated_at(updated_at) {}

std::string UserModel::get_id() const { return _id; }
std::string UserModel::get_community_id() const { return _community_id; }
std::string UserModel::get_username() const { return _username; }
std::string UserModel::get_email() const { return _email; }
std::string UserModel::get_type() const { return _type; }
int UserModel::get_balance() const { return _balance; }
std::string UserModel::get_created_at() const { return _created_at; }
std::string UserModel::get_updated_at() const { return _updated_at; }

std::unique_ptr<UserModel> UserModel::create_user(pqxx::connection& db, const std::string& community_id, const std::string& username, const std::string& email, const std::string& password, const std::string& type, const int balance, bool throw_when_null) {
	pqxx::work txn(db);

	pqxx::result result = txn.exec_params("INSERT INTO users (community_id, username, email, password, type, balance) VALUES ($1, $2, $3, $4, $5, $6) RETURNING id, community_id, username, email, type, balance, created_at, updated_at", community_id, username, email, password, type, balance);

	txn.commit();

	if (result.empty()) {
		if (throw_when_null)
			throw creation_exception("user could not be created");
		else
			return nullptr;
	}

	return std::make_unique<UserModel>(
		result[0]["id"].as<std::string>(),
		result[0]["community_id"].as<std::string>(),
		result[0]["username"].as<std::string>(),
		result[0]["email"].as<std::string>(),
		result[0]["type"].as<std::string>(),
		result[0]["balance"].as<int>(),
		result[0]["created_at"].as<std::string>(),
		result[0]["updated_at"].as<std::string>());
}

std::vector<std::unique_ptr<UserModel>> UserModel::get_users(pqxx::connection& db) {
	std::vector<std::unique_ptr<UserModel>> all_users;

	pqxx::work txn(db);

	pqxx::result result = txn.exec("SELECT id, community_id, username, email, type, balance, created_at, updated_at FROM users");

	txn.commit();

	for (const auto& row : result) {
		all_users.push_back(std::make_unique<UserModel>(
			row["id"].as<std::string>(),
			row["community_id"].as<std::string>(),
			row["username"].as<std::string>(),
			row["email"].as<std::string>(),
			row["type"].as<std::string>(),
			row["balance"].as<int>(),
			row["created_at"].as<std::string>(),
			row["updated_at"].as<std::string>()));
	}

	return all_users;
}

bool exists_user(pqxx::connection& db, const std::string& column, const std::string& value) {
	try {
		pqxx::work txn(db);
		pqxx::result result = txn.exec_params(std::format("SELECT id FROM users WHERE {} = $1", column), value);
		txn.commit();
		bool userExists = !result.empty() && !result[0][0].is_null();
		return userExists;
	} catch (const std::exception& e) {
		return false;
	}
}

bool UserModel::exists_id(pqxx::connection& db, const std::string& id) {
	return exists_user(db, "id", id);
}
bool UserModel::exists_username(pqxx::connection& db, const std::string& username) {
	return exists_user(db, "username", username);
}
bool UserModel::exists_email(pqxx::connection& db, const std::string& email) {
	return exists_user(db, "email", email);
}

std::unique_ptr<UserModel> get_user(pqxx::connection& db, const std::string& column, const std::string& value, bool throw_when_null) {
	pqxx::work txn(db);
	pqxx::result result = txn.exec_params(std::format("SELECT id, community_id, username, email, type, balance, created_at, updated_at FROM users WHERE {} = $1", column), value);
	txn.commit();

	if (result.empty()) {
		if (throw_when_null)
			throw data_not_found_exception("user not found");
		else
			return nullptr;
	}

	return std::make_unique<UserModel>(
		result[0]["id"].as<std::string>(),
		result[0]["community_id"].as<std::string>(),
		result[0]["username"].as<std::string>(),
		result[0]["email"].as<std::string>(),
		result[0]["type"].as<std::string>(),
		result[0]["balance"].as<int>(),
		result[0]["created_at"].as<std::string>(),
		result[0]["updated_at"].as<std::string>());
}

std::unique_ptr<UserModel> UserModel::get_user_by_id(pqxx::connection& db, const std::string& id, bool throw_when_null) {
	return get_user(db, "id", id, throw_when_null);
}
std::unique_ptr<UserModel> UserModel::get_user_by_username(pqxx::connection& db, const std::string& username, bool throw_when_null) {
	return get_user(db, "username", username, throw_when_null);
}
std::unique_ptr<UserModel> UserModel::get_user_by_email(pqxx::connection& db, const std::string& email, bool throw_when_null) {
	return get_user(db, "email", email, throw_when_null);
}

std::string UserModel::get_password_by_email(pqxx::connection& db, const std::string& email) {
	try {
		pqxx::work txn(db);
		pqxx::result result = txn.exec_params("SELECT password FROM users WHERE email = $1", email);
		txn.commit();
		if (result.empty()) return "";
		return result[0]["password"].as<std::string>();
	} catch (const std::exception& e) {
		return "";
	}
}

bool UserModel::delete_user_by_id(pqxx::connection& db, const std::string& id) {
	try {
		pqxx::work txn(db);

		pqxx::result result = txn.exec_params("DELETE FROM users WHERE id = $1 RETURNING id", id);

		txn.commit();

		if (!result.empty() && !result[0][0].is_null()) return true;

		return false;
	} catch (const std::exception& e) {
		std::cerr << "Failed to delete user: " << e.what() << std::endl;
		return false;
	}
}

bool UserModel::update_user_by_id(pqxx::connection& db, const std::string& id, const std::string username, const std::string email, const std::string password) {
	try {
		pqxx::work txn(db);
		if (username != "") pqxx::result result = txn.exec_params("UPDATE users SET username = $1 WHERE id = $2", username, id);
		if (email != "") pqxx::result result = txn.exec_params("UPDATE users SET email = $1 WHERE id = $2", email, id);
		if (password != "") pqxx::result result = txn.exec_params("UPDATE users SET password = $1 WHERE id = $2", password, id);
		txn.commit();
		return true;
	} catch (const std::exception& e) {
		std::cerr << "Failed to update user: " << e.what() << std::endl;
		return false;
	}
}

bool UserModel::update_user_admin(pqxx::connection& db, const std::string& id, const std::string username, const int balance) {
	try {
		pqxx::work txn(db);
		if (username != "") pqxx::result result = txn.exec_params("UPDATE users SET username = $1 WHERE id = $2", username, id);
		if (!(balance < 0)) pqxx::result result = txn.exec_params("UPDATE users SET balance = $1 WHERE id = $2", balance, id);
		txn.commit();
		return true;
	} catch (const std::exception& e) {
		std::cerr << "Failed to update user: " << e.what() << std::endl;
		return false;
	}
}
