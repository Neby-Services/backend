#include <controllers/notification_controller.h>

void NotificationController::create_notification(pqxx::connection& db, const crow::request& req, crow::response& res, const std::string& service_id) {
	try {
		//? get id that creates notification to get service
		crow::json::rvalue body = crow::json::load(req.body);
		std::string notifier_id = body["id"].s();
		int notifier_balance = body["request_balance"].i();

		std::cout << "balancation " << notifier_balance << std::endl;
		std::cout << "id notfiier " << notifier_id << std::endl;

		//? check if service exist
		std::unique_ptr<ServiceModel> service = ServiceModel::get_service_by_id(db, service_id);
		if (!service) {
			handle_error(res, "service not found", 404);
			return;
		}

		//? check that the notifier has not previously requested the service
		if (NotificationModel::is_requested(db, notifier_id)) {
			handle_error(res, "you cannot request the service again", 400);
			return;
		}

		//? check if the notifier has enough money
		if (notifier_balance < service.get()->get_price()) {
			handle_error(res, "there is not enough balance", 400);
			return;
		}

		//? check if service is in community of solicitant
		std::string notifier_community_id = body["request_community_id"].s();

		std::string service_creator_id = service.get()->get_creator_id();
		std::unique_ptr<UserModel> creator = UserModel::get_user_by_id(db, service_creator_id);
		std::string service_community = creator.get()->get_community_id();
		if (notifier_community_id != service_community) {
			handle_error(res, "does not belong to your community", 400);
			return;
		}

		//? check if notifier is not the creator of service
		if (notifier_id == service_creator_id) {
			handle_error(res, "you cannot request your own service", 400);
			return;
		}

		//? create notification state PENDING
		std::unique_ptr<NotificationModel> new_notification = NotificationModel::create_notification(db, notifier_id, service_id, NotificationStatus::PENDING, true);

		//? res.status_code = 201;
		//? res.body = { "id", "sender_id", "service_id", "status", "created_at", "updated_at" }
		//! update data.sql for support updated_at automatically

		crow::json::wvalue data;
		data["id"] = new_notification.get()->get_id();
		data["sender_id"] = new_notification.get()->get_sender_id();
		data["service_id"] = new_notification.get()->get_service_id();
		data["status"] = new_notification.get()->get_status();
		data["created_at"] = new_notification.get()->get_created_at();
		data["updated_at"] = new_notification.get()->get_updated_at();

		res.code = 201;
		res.write(data.dump());
		res.end();
	} catch (const std::exception& e) {
		std::cerr << "Error in create_notification: " << e.what() << std::endl;
		handle_error(res, "internal server errror", 500);
	}
}

void NotificationController::handle_notification(pqxx::connection& db, const crow::request& req, crow::response& res, const std::string& notification_id) {
	try {
		//? extarct query string param -> action = accepeted | refused
		auto action = req.url_params.get("action");
		crow::json::rvalue body = crow::json::load(req.body);
		std::string request_id = body["id"].s();

		//? check if action query param exists
		if (!action) {
			handle_error(res, "string query param (action) not provided", 400);
			return;
		}

		//? check if action = accepted || refused
		if (!(std::string(action) == NotificationStatus::ACCEPTED || std::string(action) == NotificationStatus::REFUSED)) {
			handle_error(res, "action not valid value", 400);
			return;
		}

		//? check if the notificaction exists
		std::unique_ptr notification = NotificationModel::get_notification_by_id(db, notification_id);

		if (!notification) {
			handle_error(res, "notification not found", 400);
			return;
		}

		//? check if the user making the request is the creator of the service
		std::unique_ptr<ServiceModel> service = ServiceModel::get_service_by_id(db, notification.get()->get_service_id(), true);

		if (request_id != service.get()->get_creator_id()) {
			handle_error(res, "you are not the creator of the service", 400);
			return;
		}

		//? if action == accepted -> accept the notification and refused others

		std::unique_ptr<NotificationModel> updated_notification;

		if (action == NotificationStatus::REFUSED) {
			updated_notification = NotificationModel::handle_notification_status(db, NotificationStatus::REFUSED, notification_id, true);
		} else {
			std::unique_ptr<UserModel> notificationCreator = UserModel::get_user_by_id(db, notification->get_sender_id());
			std::unique_ptr<UserModel> serviceCreator = UserModel::get_user_by_id(db, service->get_creator_id());

			if (service->get_type() == "offered") {
				if (notificationCreator->get_balance() < service->get_price()) {
					handle_error(res, "Notification sender does not have enough coins to pay for the service", 400);
					return;
				}
				else {
					int new_sender_balance = notificationCreator->get_balance() - service->get_price();
					int new_creator_balance = serviceCreator->get_balance() + service->get_price();
					UserModel::update_user_admin(db, notificationCreator->get_id(), notificationCreator->get_username(), new_sender_balance);
					UserModel::update_user_admin(db, serviceCreator->get_id(), serviceCreator->get_username(), new_creator_balance);
				}
			}
			else {
				if (serviceCreator->get_balance() < service->get_price()) {
					handle_error(res, "You don't have the coins to pay for this request", 400);
					return;
				}
				else {
					int new_sender_balance = notificationCreator->get_balance() + service->get_price();
					int new_creator_balance = serviceCreator->get_balance() - service->get_price();
					UserModel::update_user_admin(db, notificationCreator->get_id(), notificationCreator->get_username(), new_sender_balance);
					UserModel::update_user_admin(db, serviceCreator->get_id(), serviceCreator->get_username(), new_creator_balance);
				}
			}
			updated_notification = NotificationModel::handle_notification_status(db, NotificationStatus::ACCEPTED, notification_id, true);

			bool succes_refused = NotificationModel::refused_notifications(db, updated_notification.get()->get_service_id(), notification_id);

			if (!succes_refused) {
				handle_error(res, "error in refused other notifications", 400);
				return;
			}
		}

		//? if action == refused -> refuse the notification
		std::cout << action << std::endl;
		res.code = 200;
		crow::json::wvalue data;
		data["id"] = updated_notification.get()->get_id();
		data["sender_id"] = updated_notification.get()->get_sender_id();
		data["service_id"] = updated_notification.get()->get_service_id();
		data["status"] = updated_notification.get()->get_status();
		data["created_at"] = updated_notification.get()->get_created_at();
		data["updated_at"] = updated_notification.get()->get_updated_at();
		res.write(data.dump());

		res.end();
	} catch (const std::exception& e) {
		std::cerr << e.what() << '\n';
	}
}
