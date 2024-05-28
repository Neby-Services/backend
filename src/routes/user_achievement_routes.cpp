#include <routes/user_achievement_routes.h>

void initialize_user_achievement_routes(NebyApp& app, ConnectionPool& pool) {
	CROW_ROUTE(app, "/api/user_achievements")
		.methods(crow::HTTPMethod::GET)
		.CROW_MIDDLEWARES(app, VerifyJWT, HandleAchievements)([&pool](crow::request& req, crow::response& res) {

		auto conn = pool.getConnection();
		UserAchievementController::get_user_achievements_self(*conn.get(), req, res);
		pool.releaseConnection(conn);
			});

	CROW_ROUTE(app, "/api/claim_achievement/<string>")
		.methods(crow::HTTPMethod::PUT)
		.CROW_MIDDLEWARES(app, VerifyJWT, HandleAchievements)([&pool](crow::request& req, crow::response& res, const std::string& arch_id) {

		auto conn = pool.getConnection();
		UserAchievementController::claim_user_achievement(*conn.get(), req, res, arch_id);
		pool.releaseConnection(conn);
			});
}
