#include <controllers/user_achievement_controller.h>

void UserAchievementController::get_user_achievements_self(pqxx::connection& db, crow::request& req, crow::response& res) {
	try {
		crow::json::rvalue body = crow::json::load(req.body);

		std::string requested_id = body["id"].s();

		std::vector<std::unique_ptr<UserAchievementModel>> user_achievements = UserAchievementModel::get_user_achievements_by_id(db, requested_id, "" , true);

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
		std::vector<std::string> vec = {AchievementsTitles::ACHIEVEMENT_ONE, AchievementsTitles::ACHIEVEMENT_TWO, AchievementsTitles::ACHIEVEMENT_THREE};

		set_new_body_prop(req, "tags", vec);
		set_new_body_prop(req, "primary_user_id", requested_id);

		res.code = 200;
		res.write(data.dump());
		res.end();
	} catch (const std::exception& e) {
		std::cerr << "Error in get user achievements self: " << e.what() << std::endl;
		handle_error(res, "internal server error", 500);
	}
}
