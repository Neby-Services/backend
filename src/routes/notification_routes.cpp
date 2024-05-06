#include <routes/notification_routes.h>

void initialize_notifications_routes(NebyApp& app, pqxx::connection& db) {
	CROW_ROUTE(app, "/api/notifications/<string>")
		.methods(crow::HTTPMethod::POST)
		//.CROW_MIDDLEWARES(app, VerifyJWT)
		
		([&db](const crow::request& req, crow::response& res, const std::string& service_id) {
			NotificationController::create_notification(db, req, res, service_id); 
		});
}
