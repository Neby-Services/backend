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

		crow::json::wvalue data{ {"user_achievements", user_achievements_json} };

		res.code = 200;
		res.write(data.dump());
		res.end();
	}
	catch (const std::exception& e) {
		CROW_LOG_ERROR << "Error in get_user_achievements_self: " << e.what();
		handle_error(res, "internal server error", 500);
	}
}

void UserAchievementController::claim_user_achievement(pqxx::connection& db, crow::request& req, crow::response& res, const std::string& arch_id) {
	try {
		crow::json::rvalue body = crow::json::load(req.body);

		std::string user_id = body["id"].s();

		std::unique_ptr<UserAchievementModel> updated_achievement = UserAchievementModel::update_status_by_id(db, arch_id, "claimed", true);

		if (updated_achievement) {
			int reward = updated_achievement->get_achievement().value().get_reward();

			UserAchievementModel::update_user_balance(db, user_id, reward);
			res.code = 200;
		}
		else {

			handle_error(res, "error updating", 404);
		}
		res.end();
	}
	catch (const std::exception& e) {
		CROW_LOG_ERROR << "Error in claim_user_achievement: " << e.what();
		handle_error(res, "internal server error", 500);
	}
}
