#include <models/user_achievement_model.h>

UserAchievementModel::UserAchievementModel(std::string id, std::string achievement_title, std::string user_id, std::string status, std::optional<AchievementModel> achievement) : _id(id), _achievement_title(achievement_title), _user_id(user_id), _status(status), _achievement(achievement) {}

// * GETTERS

std::string UserAchievementModel::get_id() const { return _id; }
std::string UserAchievementModel::get_achievement_title() const { return _achievement_title; }
std::string UserAchievementModel::get_user_id() const { return _user_id; }
std::string UserAchievementModel::get_status() const { return _status; }
std::optional<AchievementModel> UserAchievementModel::get_achievement() const { return _achievement; }

// * ------------------------------------------------------------------------------

std::vector<std::unique_ptr<UserAchievementModel>> UserAchievementModel::create_user_achievement_tables(pqxx::connection& db, const std::vector<std::string>& achievements_titles, const std::string& user_id, bool throw_when_null) {
	std::vector<std::unique_ptr<UserAchievementModel>> all_user_achievements;

	pqxx::work txn(db);

	// Iterating through achievement titles
	for (const auto& title : achievements_titles) {
		// Inserting user achievement relation
		pqxx::result result = txn.exec_params("INSERT INTO user_achievements (user_id, achievement_title) VALUES ($1, $2) RETURNING id, user_id, achievement_title, status", user_id, title);

		// Checking if the insertion succeeded
		if (result.empty()) {
			if (throw_when_null) {
				throw std::runtime_error("Failed to insert user achievement");
			}
			else {
				continue;
			}
		}

		// Retrieving inserted user achievement data
		auto row = result[0];
		std::string id = row["id"].as<std::string>();
		std::string achievement_title = row["achievement_title"].as<std::string>();
		std::string status = row["status"].as<std::string>();

		// Querying achievement details for the inserted achievement
		pqxx::result achievement_result = txn.exec_params("SELECT title, description, reward FROM achievements WHERE title = $1", achievement_title);

		// Checking if achievement details were found
		if (achievement_result.empty()) {
			if (throw_when_null) {
				throw std::runtime_error("Failed to find achievement details");
			}
			else {
				continue;
			}
		}

		// Retrieving achievement details
		auto achievement_row = achievement_result[0];
		std::string achievement_title_db = achievement_row["title"].as<std::string>();
		std::string description = achievement_row["description"].as<std::string>();
		int reward = achievement_row["reward"].as<int>();

		// Creating AchievementModel instance
		AchievementModel achievement(achievement_title_db, description, reward);

		// Creating UserAchievementModel instance
		std::unique_ptr<UserAchievementModel> user_achievement = std::make_unique<UserAchievementModel>(id, achievement_title, user_id, status, std::make_optional(achievement));

		// Adding to the result vector
		all_user_achievements.push_back(std::move(user_achievement));
	}

	// Committing transaction
	txn.commit();

	return all_user_achievements;
}

std::vector<std::unique_ptr<UserAchievementModel>> UserAchievementModel::get_user_achievements_by_id(pqxx::connection& db, const std::string& user_id, const std::string& status, bool throw_when_null) {
	std::vector<std::unique_ptr<UserAchievementModel>> user_achievements;

	pqxx::work txn(db);

	std::string query = R"(
        SELECT 
            ua.id,
            ua.user_id,
            ua.achievement_title,
            ua.status,
            a.title AS achievement_title_db,
            a.description,
            a.reward
        FROM 
            user_achievements AS ua
        INNER JOIN 
            achievements AS a ON ua.achievement_title = a.title
        WHERE 
            ua.user_id = $1
    )";

	if (!status.empty())
		query += " AND ua.status = $2";

	pqxx::result result;
	if (!status.empty())
		result = txn.exec_params(query, user_id, status);
	else
		result = txn.exec_params(query, user_id);

	txn.commit();

	if (result.empty() && throw_when_null && status.empty())
		throw data_not_found_exception("user achievements not found");

	for (const auto& row : result) {
		std::string id = row["id"].as<std::string>();
		std::string achievement_title = row["achievement_title"].as<std::string>();
		std::string status = row["status"].as<std::string>();
		std::string achievement_title_db = row["achievement_title_db"].as<std::string>();
		std::string description = row["description"].as<std::string>();
		int reward = row["reward"].as<int>();

		AchievementModel achievement(achievement_title_db, description, reward);

		std::unique_ptr<UserAchievementModel> user_achievement = std::make_unique<UserAchievementModel>(id, achievement_title, user_id, status, std::make_optional(achievement));

		user_achievements.push_back(std::move(user_achievement));
	}

	return user_achievements;
}

std::unique_ptr<UserAchievementModel> UserAchievementModel::update_status_by_id(pqxx::connection& db, const std::string& user_achievement_id, const std::string& status, bool throw_when_null){ 
	pqxx::work txn(db);

	std::string query = R"(
        UPDATE user_achievements AS ua
		SET status = $1
		FROM achievements AS a
		WHERE ua.achievement_title = a.title
		AND ua.id = $2
		RETURNING ua.*, a.*
    )";

	pqxx::result result = txn.exec_params(query, status, user_achievement_id);

	txn.commit();

	if (result.empty() && throw_when_null)
		throw data_not_found_exception("User achievement not found");

	if (!result.empty()) {
		auto row = result.front();

		std::string id = row["id"].as<std::string>();
		std::string user_id = row["user_id"].as<std::string>();
		std::string achievement_title = row["achievement_title"].as<std::string>();
		std::string new_status = row["status"].as<std::string>();
		std::string achievement_title_db = row["title"].as<std::string>();
		std::string description = row["description"].as<std::string>();
		int reward = row["reward"].as<int>();

		AchievementModel achievement(achievement_title_db, description, reward);

		return std::make_unique<UserAchievementModel>(id, achievement_title, user_id, new_status, std::make_optional(achievement));
	}

	return nullptr;
}

void UserAchievementModel::update_user_balance(pqxx::connection& db, const std::string& user_id, int reward) {
	pqxx::work txn(db);
	txn.exec_params("UPDATE users SET balance = balance + $1 WHERE id = $2", reward, user_id);
	txn.commit();
}
