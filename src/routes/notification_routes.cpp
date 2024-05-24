#include <routes/notification_routes.h>

void initialize_notifications_routes(NebyApp& app, ConnectionPool& pool) {
	CROW_ROUTE(app, "/api/notifications")
		.methods(crow::HTTPMethod::GET)
		.CROW_MIDDLEWARES(app, VerifyJWT)([&pool](const crow::request& req, crow::response& res) {
		auto conn = pool.getConnection();
		NotificationController::get_notifications(*conn.get(), req, res);
		pool.releaseConnection(conn);
			});

	CROW_ROUTE(app, "/api/services/<string>/notifications")
		.methods(crow::HTTPMethod::GET)
		.CROW_MIDDLEWARES(app, VerifyJWT)([&pool](const crow::request& req, crow::response& res, const std::string& service_id) {
		auto conn = pool.getConnection();
		NotificationController::get_notification_service_by_id(*conn.get(), req, res, service_id);
		pool.releaseConnection(conn);
			});

	CROW_ROUTE(app, "/api/notifications")
		.methods(crow::HTTPMethod::POST)
		.CROW_MIDDLEWARES(app, VerifyJWT)([&pool](const crow::request& req, crow::response& res) {
		auto conn = pool.getConnection();
		NotificationController::create_notification(*conn.get(), req, res);
		pool.releaseConnection(conn);
			});

	CROW_ROUTE(app, "/api/notifications/<string>")
		.methods(crow::HTTPMethod::PUT)
		.CROW_MIDDLEWARES(app, VerifyJWT, HandleAchievements)([&pool](crow::request& req, crow::response& res, const std::string& notification_id) {
		auto conn = pool.getConnection();
		NotificationController::handle_notification(*conn.get(), req, res, notification_id);

		pool.releaseConnection(conn);
			});
}
