#include <models/service_model.h>

ServiceModel::ServiceModel(std::string id, std::string creator_id, std::string title, std::string description, int price, std::string status, std::string type, std::string buyer_user_id) : _id(id), _creator_id(creator_id), _title(title), _description(description), _price(price), _status(status), _type(type), _buyer_user_id(buyer_user_id) {}

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
