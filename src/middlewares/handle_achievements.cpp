#include <middlewares/handle_achievements.h>

void HandleAchievements::before_handle(crow::request& req, crow::response& res, context& ctx) {}
void HandleAchievements::after_handle(crow::request& req, crow::response& res, context& ctx) {
	try {
		crow::json::rvalue body = crow::json::load(req.body);
		std::vector<std::string> tags;
		if (body.has("tags")) {
			auto& tags_array = body["tags"];
			for (size_t i = 0; i < tags_array.size(); ++i) {
				tags.push_back(tags_array[i].s());
			}
		}
		UserAchievementsHandler::handler(tags, body);

	} catch (const pqxx::data_exception& e) {
		CROW_LOG_ERROR << "PQXX execption: " << e.what();
		handle_error(res, "maybe error in query params", 400);
	} catch (const update_exception& e) {
		CROW_LOG_ERROR << "Update exception: " << e.what();
		handle_error(res, e.what(), 404);
	} catch (const std::exception& e) {
		CROW_LOG_ERROR << "Error handle_achievements middleware ( after_handle ) : " << e.what();
		handle_error(res, "internal server error", 500);
	}
}
