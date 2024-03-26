#include <models/service_model.h>

ServiceModel::ServiceModel(std::string id, std::string creator_id, std::string title, std::string description, int price, std::string status, std::string type, std::string buyer_user_id) : _id(id), _creator_id(creator_id), _title(title), _description(description), _price(price), _status(status), _type(type), _buyer_user_id(buyer_user_id) {}

ServiceModel::ServiceModel(std::string id, std::string creator_id, std::string title, std::string description, int price, std::string status, std::string type, UserModel creator, std::string buyer_user_id)
	: _id(id), _creator_id(creator_id), _title(title), _description(description), _price(price), _status(status), _type(type), _creator(creator), _buyer_user_id(buyer_user_id) {}

std::string ServiceModel::get_id() { return _id; }
std::string ServiceModel::get_creator_id() { return _creator_id; }
std::string ServiceModel::get_title() { return _title; }
std::string ServiceModel::get_description() { return _description; }
int ServiceModel::get_price() { return _price; }
std::string ServiceModel::get_status() { return _status; }
std::string ServiceModel::get_type() { return _type; }
UserModel ServiceModel::get_creator() { return _creator; }
std::string ServiceModel::get_buyer_user_id() { return _buyer_user_id; }

// * -------------------------------------------------------------------

std::unique_ptr<ServiceModel> ServiceModel::create_service(pqxx::connection& db, std::string creator_id, std::string title, std::string description, int price, std::string type, std::string community_id, bool isThrow) {
	pqxx::work txn(db);

	pqxx::result result = txn.exec_params("INSERT INTO services (creator_id, title, description, price, type, community_id) VALUES ($1, $2, $3, $4, $5, $6) RETURNING id",
										  creator_id,
										  title,
										  description,
										  price, type,
										  community_id);

	txn.commit();

	if (result.empty()) {
		if (isThrow)
			throw data_not_found_exception("error create service model");

		else
			return nullptr;
	}

	std::string service_id = result[0]["id"].as<std::string>();

	return std::make_unique<ServiceModel>(service_id, creator_id, title, description, price, "OPEN", type);
}

std::vector<ServiceModel> ServiceModel::get_services(pqxx::connection& db, std::string community_id, std::string status) {
	std::vector<ServiceModel> all_services;
	pqxx::work txn(db);
	pqxx::result result;

	std::string query =
		"SELECT s.id, s.creator_id, s.title, s.description, s.price, s.status, s.type, u.username "
		"FROM services s "
		"INNER JOIN users u ON s.creator_id = u.id";

	if (!community_id.empty()) {
		query += " WHERE u.community_id = '" + community_id + "'";
		if (!status.empty())
			query += " AND s.status = '" + status + "'";
	} else if (!status.empty()) {
		query += " WHERE s.status = '" + status + "'";
	}

	result = txn.exec(query);

	for (const auto& row : result) {
		// Crear un objeto UserModel para el creador
		UserModel creator(row["creator_id"].as<std::string>(), row["username"].as<std::string>());

		// Crear el objeto ServiceModel con los datos del servicio y el creador
		ServiceModel service(row["id"].as<std::string>(), row["creator_id"].as<std::string>(), row["title"].as<std::string>(), row["description"].as<std::string>(), row["price"].as<int>(), row["status"].as<std::string>(), row["type"].as<std::string>(), creator);

		all_services.push_back(service);
	}

	txn.commit();
	return all_services;
}
