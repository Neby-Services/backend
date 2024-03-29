#include <models/service_model.h>

ServiceModel::ServiceModel(std::string id, std::string community_id, std::string creator_id, std::optional<std::string> buyer_id, std::string title, std::string description, int price, std::string status, std::string type, std::optional<std::string> image_url, std::string created_at, std::string updated_at) : _id(id), _community_id(community_id), _creator_id(creator_id), _buyer_id(buyer_id), _title(title), _description(description), _price(price), _status(status), _type(type), _image_url(image_url), _created_at(created_at), _updated_at(updated_at) {}

std::string ServiceModel::get_id() const { return _id; }
std::string ServiceModel::get_community_id() const { return _community_id; }
std::string ServiceModel::get_creator_id() const { return _creator_id; }
std::optional<std::string> ServiceModel::get_buyer_id() const { return _buyer_id; }
std::string ServiceModel::get_title() const { return _title; }
std::string ServiceModel::get_description() const { return _description; }
int ServiceModel::get_price() const { return _price; }
std::string ServiceModel::get_status() const { return _status; }
std::string ServiceModel::get_type() const { return _type; }
std::optional<std::string> ServiceModel::get_image_url() const { return _image_url; }
std::string ServiceModel::get_created_at() const { return _created_at; }
std::string ServiceModel::get_updated_at() const { return _updated_at; }

std::unique_ptr<ServiceModel> ServiceModel::create_service(pqxx::connection& db, const std::string& community_id, const std::string& creator_id, const std::string& title, const std::string& description, const int price, const std::string& type, const std::optional<std::string>& image_url, bool isThrow) {
	pqxx::work txn(db);

	pqxx::result result = txn.exec_params("INSERT INTO services (community_id, creator_id, title, description, price, type) VALUES ($1, $2, $3, $4, $5, $6) RETURNING id, community_id, creator_id, buyer_id, title, description, price, status, type, image_url, created_at, updated_at", community_id, creator_id, title, description, price, type);

	txn.commit();

	if (result.empty()) {
		if (isThrow)
			throw creation_exception("service could not be created");
		else
			return nullptr;
	}

	std::optional<std::string> buyer_id_field;
	std::optional<std::string> image_url_field;
	if (!result[0]["buyer_id"].is_null())
		buyer_id_field = result[0]["buyer_id"].as<std::string>();
	else
		buyer_id_field = std::nullopt;
	if (!result[0]["image_url"].is_null())
		image_url_field = result[0]["image_url"].as<std::string>();
	else
		image_url_field = std::nullopt;

	return std::make_unique<ServiceModel>(
		result[0]["id"].as<std::string>(),
		result[0]["community_id"].as<std::string>(),
		result[0]["creator_id"].as<std::string>(),
		buyer_id_field,
		result[0]["title"].as<std::string>(),
		result[0]["description"].as<std::string>(),
		result[0]["price"].as<int>(),
		result[0]["status"].as<std::string>(),
		result[0]["type"].as<std::string>(),
		image_url_field,
		result[0]["created_at"].as<std::string>(),
		result[0]["updated_at"].as<std::string>());
}

std::vector<std::unique_ptr<ServiceModel>> ServiceModel::get_open_services_by_community_id(pqxx::connection& db, const std::string& community_id) {
	std::vector<std::unique_ptr<ServiceModel>> all_services;

	pqxx::work txn(db);

	pqxx::result result = txn.exec("SELECT id, community_id, creator_id, buyer_id, title, description, price, status, type, image_url, created_at, updated_at FROM services");

	txn.commit();

	for (const auto& row : result) {
		std::optional<std::string> buyer_id_field;
		std::optional<std::string> image_url_field;
		if (!row["buyer_id"].is_null())
			buyer_id_field = row["buyer_id"].as<std::string>();
		else
			buyer_id_field = std::nullopt;
		if (!row["image_url"].is_null())
			image_url_field = row["image_url"].as<std::string>();
		else
			image_url_field = std::nullopt;

		all_services.push_back(std::make_unique<ServiceModel>(
			row["id"].as<std::string>(),
			row["community_id"].as<std::string>(),
			row["creator_id"].as<std::string>(),
			buyer_id_field,
			row["title"].as<std::string>(),
			row["description"].as<std::string>(),
			row["price"].as<int>(),
			row["status"].as<std::string>(),
			row["type"].as<std::string>(),
			image_url_field,
			row["created_at"].as<std::string>(),
			row["updated_at"].as<std::string>()));
	}

	return all_services;
}
