#include <middlewares/handle_achievements.h>

void HandleAchievements::before_handle(crow::request& req, crow::response& res, context& ctx) {}
void HandleAchievements::after_handle(crow::request& req, crow::response& res, context& ctx) {
	crow::json::rvalue body = crow::json::load(req.body);
	std::vector<std::string> tags;
	if (body.has("tags")) {
		auto& tags_array = body["tags"];
		for (size_t i = 0; i < tags_array.size(); ++i) {
			tags.push_back(tags_array[i].s());
		}
	}
	UserAchievementsHandler::handler(tags, body);
}
