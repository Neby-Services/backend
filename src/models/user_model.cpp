#include "models/user_model.h"

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

void UserModel::set_community_id(pqxx::connection& db, std::string community_id, std::string user_id) {
	try {
		pqxx::work txn(db);

		// Actualizar el community_id del usuario en la base de datos
		txn.exec_params("UPDATE users SET community_id = $1 WHERE id = $2", community_id, user_id);

		txn.commit();
	} catch (const std::exception& e) {
		std::cerr << "Error al establecer el community_id para el usuario " << user_id << ": " << e.what() << std::endl;
	}
}

std::unique_ptr<UserModel> UserModel::get_user_by_email(pqxx::connection& db, std::string email) {
	try {
		pqxx::work txn(db);

		pqxx::result result = txn.exec_params("SELECT id, email, username, image_url, balance, type FROM users WHERE email = $1", email);

		txn.commit();

		// Si no se encontró ningún usuario con ese correo electrónico, devolver nullptr
		if (result.empty())
			return nullptr;

		// Si se encontró un usuario, crear una instancia de UserModel y devolverla
		pqxx::row row = result[0];
		return std::make_unique<UserModel>(row["id"].as<std::string>(),
										   row["email"].as<std::string>(),
										   row["username"].as<std::string>(),
										   row["image_url"].as<std::string>(),
										   row["balance"].as<int>(),
										   row["type"].as<std::string>());
	} catch (const std::exception& e) {
		std::cerr << "Error al obtener el usuario por correo electrónico: " << e.what() << std::endl;
		return nullptr;
	}
}

std::string UserModel::get_password_by_email(pqxx::connection& db, std::string email) {
	try {
		pqxx::work txn(db);

		pqxx::result result = txn.exec_params("SELECT password FROM users WHERE email = $1", email);

		txn.commit();

		// Si no se encontró ningún usuario con ese correo electrónico, devolver una cadena vacía
		if (result.empty())
			return "";

		// Si se encontró un usuario, devolver su contraseña
		return result[0]["password"].as<std::string>();
	} catch (const std::exception& e) {
		std::cerr << "Error al obtener la contraseña por correo electrónico: " << e.what() << std::endl;
		return "";
	}
}

UserModel UserModel::get_user_by_id(pqxx::connection& db, const std::string& id) {
	pqxx::work txn(db);
	pqxx::result result = txn.exec_params("SELECT id, email, username, image_url, balance, type FROM users WHERE id = $1", id);
	if(result.empty()) throw data_not_found_exception("user doesn't exist");
	const auto& row = result[0];
	UserModel user(row["id"].as<std::string>(), row["email"].as<std::string>(), row["username"].as<std::string>(), row["image_url"].as<std::string>(), row["balance"].as<int>(), row["type"].as<std::string>());

	txn.commit();
	return user;
}

bool UserModel::delete_by_id(pqxx::connection &db, const std::string& id) {
    try {
        pqxx::work txn(db);

        pqxx::result result = txn.exec_params("DELETE FROM users WHERE id = $1", id);

        txn.commit();

        return true;
    } catch (const std::exception &e) {
        std::cerr << "Failed to delete user: " << e.what() << std::endl;
        return false; 
    }
}
