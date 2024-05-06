#include <controllers/notification_controller.h>

void NotificationController::create_notification(pqxx::connection& db, const crow::request& req, crow::response& res, const std::string& service_id) {
	try {
		//? get id that creates notification to get service
		crow::json::rvalue body = crow::json::load(req.body);
		std::string notifier_id = body["id"].s();
		//? check if service exist
		std::unique_ptr<ServiceModel> service = ServiceModel::get_service_by_id(db, service_id);
		if (!service) {
			handle_error(res, "service not found", 404);
			return;
		}
		//? check if service is in community of solicitant
		
		//? create notification state PENDING
		//? res.status_code = 201;
		//? res.body = { "id", "sender_id", "service_id", "status", "created_at", "updated_at" }
		//? update data.sql for support updated_at automatically

		crow::json::wvalue data({
			{"message",
			 service_id},
		});

		res.code = 201;
		res.write(data.dump());
		res.end();
	} catch (const std::exception& e) {
		std::cerr << "Error in create_notification: " << e.what() << std::endl;
		handle_error(res, "internal server errror", 500);
	}
}
