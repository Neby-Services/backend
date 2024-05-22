#include <routes/user_achievement_routes.h>

void initialize_user_achievement_routes(NebyApp& app) {
	ConnectionPool& pool = ConnectionPool::getInstance(connection_string, 100);
	CROW_ROUTE(app, "/api/user_achievements")
		.methods(crow::HTTPMethod::GET)
		.CROW_MIDDLEWARES(app, VerifyJWT, HandleAchievements)([&pool](crow::request& req, crow::response& res) {

		auto conn = pool.getConnection();
		UserAchievementController::get_user_achievements_self(*conn.get(), req, res);
		pool.releaseConnection(conn);
			});
}
