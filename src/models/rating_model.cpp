#include <models/rating_model.h>

RatingModel::RatingModel(std::string id, std::string service_id, int rating, std::string description) : _id(id), _service_id(service_id), _rating(rating), _description(description) {}

RatingModel::RatingModel(std::string id, std::string service_id, int rating, std::string description, std::optional<std::string> sender_id) : _id(id), _service_id(service_id), _rating(rating), _description(description), _sender_id(sender_id) {}

std::string RatingModel::get_id() const { return _id; }
std::string RatingModel::get_service_id() const { return _service_id; }
int RatingModel::get_rating() const { return _rating; }
std::string RatingModel::get_description() const { return _description; }
std::optional<std::string> RatingModel::get_sender_id() const { return _sender_id; }

std::unique_ptr<RatingModel> RatingModel::create_rating(pqxx::connection& db, const std::string& service_id, const int& rating, const std::string& description, bool throw_when_null) {
	try {
		pqxx::work txn(db);

		pqxx::result result = txn.exec_params("INSERT INTO ratings (service_id, rating, description) VALUES ($1, $2, $3) RETURNING id, service_id, rating, description, created_at, updated_at", service_id, rating, description);

		txn.commit();

		if (result.empty()) {
			if (throw_when_null)
				throw creation_exception("rating could not be created");
			else
				return nullptr;
		}

		return std::make_unique<RatingModel>(
			result[0]["id"].as<std::string>(),
			result[0]["service_id"].as<std::string>(),
			result[0]["rating"].as<int>(),
			result[0]["description"].as<std::string>());
	} catch (const std::exception& e) {
		std::cerr << "Error to create rating: " << e.what() << std::endl;
		return nullptr;
	}
}

bool RatingModel::update_rating(pqxx::connection& db, const std::string& rating_id, const int& rating, const std::string& description, bool throw_when_null) {
	if (rating_id.empty() && rating == 0 && description.empty()) {
		throw update_exception("nothing has been updated, there is no data to update");
	}

	pqxx::work txn(db);

	std::string query =
		"UPDATE ratings "
		"SET rating = $2 "
		"WHERE id = $1";
	pqxx::result result;
	result = txn.exec_params(query, rating_id, rating);

	txn.commit();

	if (result.affected_rows() == 0) {
		if (throw_when_null)
			throw update_exception("nothing has been updated, maybe no user found to update");
		else
			return false;
	}

	return true;
}

std::vector<std::unique_ptr<RatingModel>> RatingModel::get_rating_by_user_id(pqxx::connection& db, const std::string& user_id, bool throw_when_null) {
	std::vector<std::unique_ptr<RatingModel>> all_ratings;

	pqxx::work txn(db);

	std::string query =
		"WITH service_neighbors AS ("
		"  SELECT "
		"      CASE "
		"          WHEN s.type = 'requested' THEN s.buyer_id "
		"          WHEN s.type = 'offered' THEN s.creator_id "
		"      END AS neighbor_id, "
		"      CASE "
		"          WHEN s.type = 'requested' THEN s.creator_id "
		"          WHEN s.type = 'offered' THEN s.buyer_id "
		"      END AS sender_id, "
		"      r.rating AS rating, "
		"      r.id AS id, "
		"      r.service_id AS service_id, "
		"      r.description AS description "
		"  FROM "
		"      services AS s "
		"  JOIN "
		"      ratings AS r ON r.service_id = s.id "
		"  WHERE "
		"      s.status = 'closed'"
		")"
		"SELECT "
		"    neighbor_id, "
		"    sender_id, "
		"    rating, "
		"    description, "
		"    id, "
		"    service_id "
		"FROM "
		"    service_neighbors "
		"WHERE "
		"    neighbor_id = $1";

	pqxx::result result;
	result = txn.exec_params(query, user_id);

	txn.commit();

	for (const auto& row : result) {
		all_ratings.push_back(std::make_unique<RatingModel>(
			row["id"].as<std::string>(),
			row["service_id"].as<std::string>(),
			row["rating"].as<int>(),
			row["description"].as<std::string>(),
			row["sender_id"].as<std::string>()));
	}

	return all_ratings;
}

std::unique_ptr<RatingModel> RatingModel::get_rating_by_service_id(pqxx::connection& db, const std::string& service_id, bool throw_when_null) {
	pqxx::work txn(db);

	std::string query =
		"WITH service_neighbors AS ("
		"  SELECT "
		"      CASE "
		"          WHEN s.type = 'requested' THEN s.creator_id "
		"          WHEN s.type = 'offered' THEN s.buyer_id "
		"      END AS sender_id, "
		"      r.rating AS rating, "
		"      r.id AS id, "
		"      r.service_id AS service_id, "
		"      r.description AS description "
		"  FROM "
		"      services AS s "
		"  JOIN "
		"      ratings AS r ON r.service_id = s.id "
		"  WHERE "
		"      s.status = 'closed'"
		")"
		"SELECT "
		"    sender_id, "
		"    rating, "
		"    description, "
		"    id, "
		"    service_id "
		"FROM "
		"    service_neighbors "
		"WHERE "
		"    service_id = $1";
	pqxx::result result;
	result = txn.exec_params(query, service_id);

	txn.commit();

	return std::make_unique<RatingModel>(
		result[0]["id"].as<std::string>(),
		result[0]["service_id"].as<std::string>(),
		result[0]["rating"].as<int>(),
		result[0]["description"].as<std::string>(),
		result[0]["sender_id"].as<std::string>());
}

std::unique_ptr<RatingModel> RatingModel::get_rating_by_id(pqxx::connection& db, const std::string& rating_id, bool throw_when_null) {
	pqxx::work txn(db);

	std::string query =
		"SELECT id, service_id, rating "
		"FROM ratings "
		"WHERE id = $1";
	pqxx::result result;
	result = txn.exec_params(query, rating_id);

	txn.commit();

	if (result.empty()) {
		if (throw_when_null) {
			throw data_not_found_exception("Id rating not found");
		}
		return nullptr;
	}

	return std::make_unique<RatingModel>(
		result[0]["id"].as<std::string>(),
		result[0]["service_id"].as<std::string>(),
		result[0]["rating"].as<int>(),
		result[0]["description"].as<std::string>());
}
