#include <controllers/user_achievement_controller.h>

void UserAchievementController::get_user_achievements_self(pqxx::connection& db, crow::request& req, crow::response& res) {
	try {
		crow::json::rvalue body = crow::json::load(req.body);

		std::string requested_id = body["id"].s();

		std::vector<std::unique_ptr<UserAchievementModel>> user_achievements = UserAchievementModel::get_user_achievements_by_id(db, requested_id, "", true);

		crow::json::wvalue::list user_achievements_json;
		for (unsigned int i = 0; i < user_achievements.size(); ++i) {
			crow::json::wvalue user_achievement;

			user_achievement["id"] = user_achievements[i].get()->get_id();
			user_achievement["user_id"] = user_achievements[i].get()->get_user_id();
			user_achievement["status"] = user_achievements[i].get()->get_status();

			if (user_achievements[i].get()->get_achievement().has_value()) {
				crow::json::wvalue achievement;
				achievement["title"] = user_achievements[i].get()->get_achievement().value().get_title();
				achievement["description"] = user_achievements[i].get()->get_achievement().value().get_description();
				achievement["reward"] = user_achievements[i].get()->get_achievement().value().get_reward();

				user_achievement["achievement"] = crow::json::wvalue(achievement);
			}
			user_achievements_json.push_back(user_achievement);
		}

		crow::json::wvalue data{{"user_achievements", user_achievements_json}};

		res.code = 200;
		res.write(data.dump());
		res.end();
	} catch (const pqxx::data_exception& e) {
		CROW_LOG_ERROR << "PQXX execption: " << e.what();
		handle_error(res, "invalid id", 400);
	} catch (const data_not_found_exception& e) {
		CROW_LOG_ERROR << "Data not found exception: " << e.what();
		handle_error(res, e.what(), 404);
	} catch (const std::exception& e) {
		CROW_LOG_ERROR << "Error in get_user_achievements_self: " << e.what();
		handle_error(res, "internal server error", 500);
	}
}

void UserAchievementController::claim_user_achievement(pqxx::connection& db, crow::request& req, crow::response& res, const std::string& arch_id) {
	try {
		crow::json::rvalue body = crow::json::load(req.body);

		std::string user_id = body["id"].s();

		std::map<std::string, std::string> fields = {
			{"status", AchievementStatus::CLAIMED}};

		std::unique_ptr<UserAchievementModel> user_achievement = UserAchievementModel::get_user_achievement_by_id(db, arch_id, true);

		if (user_achievement.get()->get_status() == AchievementStatus::CLAIMED) {
			handle_error(res, "you cannot claim an achievement more than once", 400);
			return;
		}

		std::unique_ptr<UserAchievementModel> updated_achievement = UserAchievementModel::update_by_id(db, arch_id, fields, true);

		if (updated_achievement->get_achievement().value().get_reward() > 0) {
			int reward = updated_achievement->get_achievement().value().get_reward();

			std::unique_ptr<UserModel> user = UserModel::get_user_by_id(db, user_id, true);

			std::map<std::string, std::string> user_update_data = {
				{"balance", std::to_string(reward + user.get()->get_balance())}};

			UserModel::update_user_by_id(db, user_id, user_update_data, true);

			res.code = 200;
			crow::json::wvalue response_message;
			response_message["message"] = "Achievement claimed successfully";
			res.write(response_message.dump());
			res.end();
		} else {
			handle_error(res, "reward no positive number", 400);
			return;
		}
	} catch (const pqxx::data_exception& e) {
		CROW_LOG_ERROR << "PQXX execption: " << e.what();
		handle_error(res, "invalid id", 400);
	} catch (const data_not_found_exception& e) {
		CROW_LOG_ERROR << "Data not found exception: " << e.what();
		handle_error(res, e.what(), 404);
	} catch (const update_exception& e) {
		CROW_LOG_ERROR << "Update exception: " << e.what();
		handle_error(res, e.what(), 404);
	} catch (const std::exception& e) {
		CROW_LOG_ERROR << "Error in claim_user_achievement: " << e.what();
		handle_error(res, "internal server error", 500);
	}
}
