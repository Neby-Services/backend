#include <models/user_model.h>

UserModel::UserModel(std::string id, std::string community_id, std::string username, std::string email, std::string type, int balance, std::string created_at, std::string updated_at, std::optional<CommunityModel> community) : _id(id), _community_id(community_id), _username(username), _email(email), _type(type), _balance(balance), _created_at(created_at), _updated_at(updated_at), _community(community) {}

std::string UserModel::get_id() const { return _id; }
std::string UserModel::get_community_id() const { return _community_id; }
std::string UserModel::get_username() const { return _username; }
std::string UserModel::get_email() const { return _email; }
std::string UserModel::get_type() const { return _type; }
int UserModel::get_balance() const { return _balance; }
std::string UserModel::get_created_at() const { return _created_at; }
std::string UserModel::get_updated_at() const { return _updated_at; }
std::optional<CommunityModel> UserModel::get_community() { return _community; }

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
		result[0]["updated_at"].as<std::string>(), std::nullopt);
}

std::vector<UserModel> UserModel::get_users_admin(pqxx::connection& db, const std::string& admin_community_id) {
	std::vector<UserModel> all_users;

	pqxx::work txn(db);

	pqxx::result result = txn.exec_params("SELECT id, community_id, username, email, type, balance, created_at, updated_at FROM users WHERE community_id = $1", admin_community_id);

	txn.commit();

	for (const auto& row : result) {
		all_users.push_back(UserModel(
			row["id"].as<std::string>(),
			row["community_id"].as<std::string>(),
			row["username"].as<std::string>(),
			row["email"].as<std::string>(),
			row["type"].as<std::string>(),
			row["balance"].as<int>(),
			row["created_at"].as<std::string>(),
			row["updated_at"].as<std::string>(), std::nullopt));
	}

	return all_users;
}

bool exists_user(pqxx::connection& db, const std::string& column, const std::string& value) {
	pqxx::work txn(db);
	pqxx::result result = txn.exec_params(std::format("SELECT id FROM users WHERE {} = $1", column), value);
	txn.commit();
	bool userExists = !result.empty() && !result[0][0].is_null();
	return userExists;
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
	pqxx::result result = txn.exec_params(
		std::format(
			"SELECT users.id, users.community_id, users.username, users.email, users.type, users.balance, users.created_at, users.updated_at, "
			"communities.id AS community_id, communities.name AS community_name, communities.code AS community_code, communities.created_at AS community_created_at, communities.updated_at AS community_updated_at "
			"FROM users "
			"JOIN communities ON users.community_id = communities.id "
			"WHERE users.{} = $1", column), value);
	txn.commit();

	if (result.empty()) {
		if (throw_when_null)
			throw data_not_found_exception("user not found");
		else
			return nullptr;
	}

	CommunityModel community(
		result[0]["community_id"].as<std::string>(),
		result[0]["community_name"].as<std::string>(),
		result[0]["community_code"].as<std::string>(),
		result[0]["community_created_at"].as<std::string>(),
		result[0]["community_updated_at"].as<std::string>()
	);

	return std::make_unique<UserModel>(
		result[0]["id"].as<std::string>(),
		result[0]["community_id"].as<std::string>(),
		result[0]["username"].as<std::string>(),
		result[0]["email"].as<std::string>(),
		result[0]["type"].as<std::string>(),
		result[0]["balance"].as<int>(),
		result[0]["created_at"].as<std::string>(),
		result[0]["updated_at"].as<std::string>(),
		community
	);
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

std::string UserModel::get_password_by_email(pqxx::connection& db, const std::string& email, bool throw_when_null) {
	pqxx::work txn(db);
	pqxx::result result = txn.exec_params("SELECT password FROM users WHERE email = $1", email);
	txn.commit();
	if (result.empty()) {
		if (throw_when_null)
			throw data_not_found_exception("user with email not found");
		else return "";
	}
	return result[0]["password"].as<std::string>();
}

bool UserModel::delete_user_by_id(pqxx::connection& db, const std::string& id, bool throw_when_null) {

	pqxx::work txn(db);

	pqxx::result result = txn.exec_params("DELETE FROM users WHERE id = $1 RETURNING id", id);

	txn.commit();

	if (result.affected_rows() == 0) {
		if (throw_when_null)
			throw deletion_exception("nothing has been deleted, maybe no user found to delete");
		else return false;
	}

	return !result.empty() && !result[0][0].is_null();
}

std::string join_user(const std::vector<std::string>& elements, const std::string& delimiter) {
	std::ostringstream os;
	auto it = elements.begin();
	if (it != elements.end()) {
		os << *it++;
	}
	while (it != elements.end()) {
		os << delimiter << *it++;
	} 
	return os.str();
}

bool UserModel::update_user_by_id(pqxx::connection& db, const std::string& id, const std::string username, const std::string email, const std::string password, int balance, bool throw_when_null) {

	pqxx::work txn(db);

	std::string query = "UPDATE users SET ";
	std::vector<std::string> updates;
	std::vector<std::string> params; // Store the actual parameters as strings

	if (!username.empty()) {
		updates.push_back("username = $" + std::to_string(updates.size() + 1));
		params.push_back(username);
	}
	if (!email.empty()) {
		updates.push_back("email = $" + std::to_string(updates.size() + 1));
		params.push_back(email);
	}
	if (!password.empty()) {
		updates.push_back("password = $" + std::to_string(updates.size() + 1));
		params.push_back(password);
	}
	if (balance >= 0) { // Asegurarse de que el balance sea no negativo
		updates.push_back("balance = $" + std::to_string(updates.size() + 1));
		params.push_back(std::to_string(balance));
	}

	if (updates.empty()) {
		// No hay nada que actualizar
		return false;
	}

	// Construir la parte de SET de la consulta
	query += join_user(updates, ", ") + " WHERE id = $" + std::to_string(updates.size() + 1);
	params.push_back(id);

	// Convert params to const char* array for exec_params
	std::vector<const char*> c_params;
	for (const auto& param : params) {
		c_params.push_back(param.c_str());
	}

	// Ejecutar la consulta
	pqxx::result result = txn.exec_params(query, pqxx::prepare::make_dynamic_params(c_params));
	txn.commit();

	if (result.affected_rows() == 0) {
		if (throw_when_null)
			throw update_exception("nothing has been updated, maybe no user found to update");
		else return false;
	}

	return true;
}
