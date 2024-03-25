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
std::string ServiceModel::get_buyer_user_id() { return _buyer_user_id; }

// * -------------------------------------------------------------------

std::unique_ptr<ServiceModel> ServiceModel::create_service(pqxx::connection& db, std::string creator_id, std::string title, std::string description, int price, std::string type, bool isThrow) {
	pqxx::work txn(db);

	pqxx::result result = txn.exec_params("INSERT INTO services (creator_id, title, description, price, type) VALUES ($1, $2, $3, $4, $5) RETURNING id",
										  creator_id,
										  title,
										  description,
										  price, type);

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

std::vector<ServiceModel> ServiceModel::get_services(pqxx::connection& db, std::string status) {
	std::vector<ServiceModel> all_services;
	pqxx::work txn(db);
	pqxx::result result;

	std::string query = "SELECT id, creator_id, title, description, price, status, type, buyer_user_id FROM services";
	if (status != "")
		query += " WHERE status = '" + status + "'";

	result = txn.exec(query);

	for (const auto& row : result) {
		std::string buyer_user_id;

		ServiceModel service(row["id"].as<std::string>(), row["creator_id"].as<std::string>(), row["title"].as<std::string>(), row["description"].as<std::string>(), row["price"].as<int>(), row["status"].as<std::string>(), row["type"].as<std::string>());

		all_services.push_back(service);
	}
		txn.commit();
	return all_services;
}
