#include <models/service_model.h>

ServiceModel::ServiceModel(std::string id, std::string creator_id, std::optional<std::string> buyer_id, std::string title, std::string description, int price, std::string status, std::string type, std::optional<std::string> image_url, std::optional<UserModel> creator, std::optional<UserModel> buyer, std::string created_at, std::string updated_at) : _id(id), _creator_id(creator_id), _buyer_id(buyer_id), _title(title), _description(description), _price(price), _status(status), _type(type), _image_url(image_url), _creator(creator), _buyer(buyer), _created_at(created_at), _updated_at(updated_at) {}

std::string ServiceModel::get_id() const { return _id; }
std::string ServiceModel::get_creator_id() const { return _creator_id; }
std::optional<std::string> ServiceModel::get_buyer_id() const { return _buyer_id; }
std::string ServiceModel::get_title() const { return _title; }
std::string ServiceModel::get_description() const { return _description; }
int ServiceModel::get_price() const { return _price; }
std::string ServiceModel::get_status() const { return _status; }
std::string ServiceModel::get_type() const { return _type; }
std::optional<std::string> ServiceModel::get_image_url() const { return _image_url; }
std::optional<UserModel> ServiceModel::get_creator() const { return _creator; };
std::optional<UserModel> ServiceModel::get_buyer() const { return _buyer; };
std::string ServiceModel::get_created_at() const { return _created_at; }
std::string ServiceModel::get_updated_at() const { return _updated_at; }

std::unique_ptr<ServiceModel> ServiceModel::create_service(pqxx::connection& db, const std::string& creator_id, const std::string& title, const std::string& description, const int price, const std::string& type, const std::optional<std::string>& image_url, bool isThrow) {
	pqxx::work txn(db);

	pqxx::result result = txn.exec_params("INSERT INTO services (creator_id, title, description, price, type) VALUES ($1, $2, $3, $4, $5) RETURNING id,  creator_id, buyer_id, title, description, price, status, type, image_url, created_at, updated_at", creator_id, title, description, price, type);

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
		result[0]["creator_id"].as<std::string>(),
		buyer_id_field,
		result[0]["title"].as<std::string>(),
		result[0]["description"].as<std::string>(),
		result[0]["price"].as<int>(),
		result[0]["status"].as<std::string>(),
		result[0]["type"].as<std::string>(),
		image_url_field,
		std::nullopt,
		std::nullopt,
		result[0]["created_at"].as<std::string>(),
		result[0]["updated_at"].as<std::string>());
}

std::vector<ServiceModel> ServiceModel::get_services(pqxx::connection& db, const std::map<std::string, std::string>& filters, bool throw_when_null) {
	std::vector<ServiceModel> all_services;

	pqxx::work txn(db);

	std::string query =
		"SELECT s.id AS service_id, "
		"s.creator_id, "
		"s.buyer_id, "
		"s.title, "
		"s.description, "
		"s.price, "
		"s.status, "
		"s.type, "
		"s.image_url, "
		"s.created_at, "
		"s.updated_at, "
		"uc.id AS creator_id, "
		"uc.community_id AS creator_community_id, "
		"uc.username AS creator_username, "
		"uc.email AS creator_email, "
		"uc.type AS creator_type, "
		"uc.balance AS creator_balance, "
		"uc.created_at AS creator_created_at, "
		"uc.updated_at AS creator_updated_at, "
		"ub.id AS buyer_id, "
		"ub.community_id AS buyer_community_id, "
		"ub.username AS buyer_username, "
		"ub.email AS buyer_email, "
		"ub.type AS buyer_type, "
		"ub.balance AS buyer_balance, "
		"ub.created_at AS buyer_created_at, "
		"ub.updated_at AS buyer_updated_at "
		"FROM services AS s "
		"JOIN users AS uc ON s.creator_id = uc.id "
		"LEFT JOIN users AS ub ON s.buyer_id = ub.id ";

	std::vector<std::string> conditions;
	std::vector<std::string> params;
	int param_count = 1;

	for (const auto& filter : filters) {
		conditions.push_back(filter.first + " = $" + std::to_string(param_count++));
		params.push_back(filter.second);
	}

	if (!conditions.empty()) {
		query += " WHERE " + join_query_update(conditions, " AND ");
	}

	query += " ORDER BY s.updated_at DESC;";

	pqxx::result result = txn.exec_params(query, pqxx::prepare::make_dynamic_params(params));
	txn.commit();

	if (result.empty() && throw_when_null) {
		throw data_not_found_exception("services not found");
	}

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

		UserModel creator(
			row["creator_id"].as<std::string>(),
			row["creator_community_id"].as<std::string>(),
			row["creator_username"].as<std::string>(),
			row["creator_email"].as<std::string>(),
			row["creator_type"].as<std::string>(),
			row["creator_balance"].as<int>(),
			row["creator_created_at"].as<std::string>(),
			row["creator_updated_at"].as<std::string>(), std::nullopt);

		std::optional<UserModel> buyer = std::nullopt;
		if (buyer_id_field) {
			buyer = UserModel(
				row["buyer_id"].as<std::string>(),
				row["buyer_community_id"].as<std::string>(),
				row["buyer_username"].as<std::string>(),
				row["buyer_email"].as<std::string>(),
				row["buyer_type"].as<std::string>(),
				row["buyer_balance"].as<int>(),
				row["buyer_created_at"].as<std::string>(),
				row["buyer_updated_at"].as<std::string>(), std::nullopt);
		}

		all_services.push_back(ServiceModel(
			row["service_id"].as<std::string>(),
			row["creator_id"].as<std::string>(),
			buyer_id_field,
			row["title"].as<std::string>(),
			row["description"].as<std::string>(),
			row["price"].as<int>(),
			row["status"].as<std::string>(),
			row["type"].as<std::string>(),
			image_url_field,
			creator,
			buyer,
			row["created_at"].as<std::string>(),
			row["updated_at"].as<std::string>()));
	}

	return all_services;
}

std::vector<ServiceModel> ServiceModel::get_services_sold_by_creator_id(pqxx::connection& db, const std::string& creator_id, bool throw_when_null) {
	std::vector<ServiceModel> all_services;

	pqxx::work txn(db);

	std::string query = R"(
            SELECT *
			FROM services
			WHERE buyer_id IS NOT NULL 
			AND creator_id = $1;
        )";

	pqxx::result result = txn.exec_params(query, creator_id);

	txn.commit();

	if (result.empty() && throw_when_null)
		throw data_not_found_exception("services not found");

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

		all_services.push_back(ServiceModel(
			row["id"].as<std::string>(),
			row["creator_id"].as<std::string>(),
			buyer_id_field,
			row["title"].as<std::string>(),
			row["description"].as<std::string>(),
			row["price"].as<int>(),
			row["status"].as<std::string>(),
			row["type"].as<std::string>(),
			image_url_field,
			std::nullopt,
			std::nullopt,
			row["created_at"].as<std::string>(),
			row["updated_at"].as<std::string>()));
	}

	return all_services;
}

std::unique_ptr<ServiceModel> get_service(pqxx::connection& db, const std::string& column, const std::string& value, bool throw_when_null) {
	pqxx::work txn(db);

	std::string query =
		"SELECT s.id AS service_id, "
		"s.creator_id, "
		"s.buyer_id, "
		"s.title, "
		"s.description, "
		"s.price, "
		"s.status, "
		"s.type, "
		"s.image_url, "
		"s.created_at, "
		"s.updated_at, "
		"uc.id AS creator_id, "
		"uc.community_id AS creator_community_id, "
		"uc.username AS creator_username, "
		"uc.email AS creator_email, "
		"uc.type AS creator_type, "
		"uc.balance AS creator_balance, "
		"uc.created_at AS creator_created_at, "
		"uc.updated_at AS creator_updated_at, "
		"ub.id AS buyer_id, "
		"ub.community_id AS buyer_community_id, "
		"ub.username AS buyer_username, "
		"ub.email AS buyer_email, "
		"ub.type AS buyer_type, "
		"ub.balance AS buyer_balance, "
		"ub.created_at AS buyer_created_at, "
		"ub.updated_at AS buyer_updated_at "
		"FROM services AS s "
		"JOIN users AS uc ON s.creator_id = uc.id "
		"LEFT JOIN users AS ub ON s.buyer_id = ub.id "
		"WHERE s." +
		column + " = $1";

	pqxx::result result = txn.exec_params(query, value);
	txn.commit();

	if (result.empty()) {
		if (throw_when_null)
			throw data_not_found_exception("service not found");
		else
			return nullptr;
	}

	const auto& row = result[0];

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

	UserModel creator(
		row["creator_id"].as<std::string>(),
		row["creator_community_id"].as<std::string>(),
		row["creator_username"].as<std::string>(),
		row["creator_email"].as<std::string>(),
		row["creator_type"].as<std::string>(),
		row["creator_balance"].as<int>(),
		row["creator_created_at"].as<std::string>(),
		row["creator_updated_at"].as<std::string>(), std::nullopt);

	std::optional<UserModel> buyer = std::nullopt;
	if (buyer_id_field) {
		buyer = UserModel(
			row["buyer_id"].as<std::string>(),
			row["buyer_community_id"].as<std::string>(),
			row["buyer_username"].as<std::string>(),
			row["buyer_email"].as<std::string>(),
			row["buyer_type"].as<std::string>(),
			row["buyer_balance"].as<int>(),
			row["buyer_created_at"].as<std::string>(),
			row["buyer_updated_at"].as<std::string>(), std::nullopt);
	}

	return std::make_unique<ServiceModel>(
		row["service_id"].as<std::string>(),
		row["creator_id"].as<std::string>(),
		buyer_id_field,
		row["title"].as<std::string>(),
		row["description"].as<std::string>(),
		row["price"].as<int>(),
		row["status"].as<std::string>(),
		row["type"].as<std::string>(),
		image_url_field,
		creator,
		buyer,
		row["created_at"].as<std::string>(),
		row["updated_at"].as<std::string>());
}

std::unique_ptr<ServiceModel> ServiceModel::get_service_by_id(pqxx::connection& db, const std::string& id, bool throw_when_null) {
	return get_service(db, "id", id, throw_when_null);
}

bool ServiceModel::update_service_by_id(pqxx::connection& db, const std::string& service_id, const std::map<std::string, std::string>& update_fields, bool throw_when_null) {
	if (update_fields.empty()) {
		if (throw_when_null)
			throw update_exception("nothing has been updated, there is no data to update");
		else
			return false;
	}

	pqxx::work txn(db);

	std::string query = "UPDATE services SET ";
	std::vector<std::string> updates;
	std::vector<std::string> params;

	int param_index = 1;
	for (const auto& field : update_fields) {
		updates.push_back(field.first + " = $" + std::to_string(param_index++));
		params.push_back(field.second);
	}

	query += join_query_update(updates, ", ") + " WHERE id = $" + std::to_string(param_index);
	params.push_back(service_id);

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
		else
			return false;
	}

	return true;
}

std::unique_ptr<ServiceModel> ServiceModel::delete_service_by_id(pqxx::connection& db, const std::string id, bool throw_when_null) {
	pqxx::work txn(db);

	pqxx::result result = txn.exec_params("DELETE FROM services WHERE id = $1 RETURNING *", id);

	txn.commit();

	if (result.affected_rows() == 0) {
		if (throw_when_null)
			throw update_exception("nothing has been deleted, maybe no user found to delete");
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
		result[0]["creator_id"].as<std::string>(),
		buyer_id_field,
		result[0]["title"].as<std::string>(),
		result[0]["description"].as<std::string>(),
		result[0]["price"].as<int>(),
		result[0]["status"].as<std::string>(),
		result[0]["type"].as<std::string>(),
		image_url_field,
		std::nullopt,
		std::nullopt,
		result[0]["created_at"].as<std::string>(),
		result[0]["updated_at"].as<std::string>());
}

