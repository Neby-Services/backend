#include <controllers/notification_controller.h>

void NotificationController::create_notification(pqxx::connection& db, const crow::request& req, crow::response& res) {
	try {
		auto notification_type = req.url_params.get("type");

		if (!notification_type) {
			handle_error(res, "string query param (type) not provided", 400);
			return;
		}

		if (!(notification_type == NotificationType::ACHIEVEMENTS || notification_type == NotificationType::REVIEWS || notification_type == NotificationType::SERVICES)) {
			handle_error(res, "incorrect value for type string query param", 400);
			return;
		}

		if (notification_type == NotificationType::SERVICES) {
			NotificationController::create_service_notification(db, req, res);
			return;
		}
		else if (notification_type == NotificationType::ACHIEVEMENTS) {
			res.code = 201;
			res.body = "here handle achievements";
			return;
		}
		else if (notification_type == NotificationType::REVIEWS) {
			res.code = 201;
			res.body = "here handle reviews";
			return;
		}


	}
	catch (const pqxx::data_exception& e) {
		CROW_LOG_ERROR << "PQXX execption: " << e.what();
		handle_error(res, "invalid id", 400);
	}
	catch (const data_not_found_exception& e) {
		CROW_LOG_ERROR << "Data not found exception: " << e.what();
		handle_error(res, e.what(), 404);
	}
	catch (const creation_exception& e) {
		CROW_LOG_ERROR << "Create exception: " << e.what();
		handle_error(res, e.what(), 404);
	}
	catch (const std::exception& e) {
		CROW_LOG_ERROR << "Error in create_notification controller: " << e.what();
		handle_error(res, "internal server errror", 500);
	}
}

void NotificationController::create_service_notification(pqxx::connection& db, const  crow::request& req, crow::response& res) {
	try
	{
		//? get id that creates notification to get service
		crow::json::rvalue body = crow::json::load(req.body);
		int notifier_balance = body["request_balance"].i();
		std::string notifier_id = body["id"].s();

		auto service_id = req.url_params.get("service_id");

		if (!service_id) {
			handle_error(res, "string query param (service) not provided", 400);
			return;
		}

		//? check if service exist
		std::unique_ptr<ServiceModel> service = ServiceModel::get_service_by_id(db, service_id);
		if (!service) {
			handle_error(res, "service not found", 404);
			return;
		}

		//? check that the notifier has not previously requested the service
		if (NotificationServiceModel::is_requested(db, notifier_id, service_id)) {
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
		std::unique_ptr<NotificationModel> new_notification = NotificationModel::create_notification(db, NotificationType::SERVICES, true);
		std::unique_ptr<NotificationServiceModel> new_notification_service = NotificationServiceModel::create_notification_service(db, new_notification.get()->get_id(), notifier_id, service_id, NotificationServicesStatus::PENDING, true);

		//? res.status_code = 201;
		//? res.body = { "id", "sender_id", "service_id", "status", "created_at", "updated_at" }
		//! update data.sql for support updated_at automatically

		crow::json::wvalue notification_service_json;
		notification_service_json["id"] = new_notification_service.get()->get_id();
		notification_service_json["sender_id"] = new_notification_service.get()->get_sender_id();
		notification_service_json["service_id"] = new_notification_service.get()->get_service_id();
		notification_service_json["status"] = new_notification_service.get()->get_status();
		notification_service_json["notification_id"] = new_notification_service.get()->get_notification_id();

		crow::json::wvalue notification_json;
		notification_json["id"] = new_notification.get()->get_id();
		notification_json["type"] = new_notification.get()->get_type();
		notification_json["created_at"] = new_notification.get()->get_created_at();
		notification_json["updated_at"] = new_notification.get()->get_updated_at();
		notification_json["notification_service"] = crow::json::wvalue(notification_service_json);


		res.code = 201;
		res.write(notification_json.dump());
		res.end();
	}
	catch (const pqxx::data_exception& e) {
		CROW_LOG_ERROR << "PQXX execption: " << e.what();
		handle_error(res, "invalid id", 400);
	}
	catch (const data_not_found_exception& e) {
		CROW_LOG_ERROR << "Data not found exception: " << e.what();
		handle_error(res, e.what(), 404);
	}
	catch (const std::exception& e)
	{
		CROW_LOG_ERROR << "Error in create_service_notification controller: " << e.what();
		handle_error(res, "internal server errror", 500);
	}
}


void NotificationController::get_notification_service_by_id(pqxx::connection& db, const crow::request& req, crow::response& res, const std::string& service_id) {
	try
	{
		crow::json::rvalue body = crow::json::load(req.body);
		std::string request_id = body["id"].s();

		std::unique_ptr<NotificationModel> notification = NotificationModel::get_notification_service_by_id(db, request_id, service_id);

		if (!notification) {
			handle_error(res, "the service has not been requested yet", 400);
			return;
		}

		res.code = 200;
		crow::json::wvalue notification_json;
		notification_json["id"] = notification.get()->get_id();
		notification_json["type"] = notification.get()->get_type();
		notification_json["created_at"] = notification.get()->get_created_at();
		notification_json["updated_at"] = notification.get()->get_updated_at();

		crow::json::wvalue notification_service_json;
		notification_service_json["id"] = notification.get()->get_notification_service().value().get_id();
		notification_service_json["sender_id"] = notification.get()->get_notification_service().value().get_sender_id();
		notification_service_json["service_id"] = notification.get()->get_notification_service().value().get_service_id();
		notification_service_json["status"] = notification.get()->get_notification_service().value().get_status();
		notification_service_json["notification_id"] = notification.get()->get_notification_service().value().get_notification_id();

		notification_json["notification_service"] = crow::json::wvalue(notification_service_json);

		res.write(notification_json.dump());

		res.end();
	}
	catch (const pqxx::data_exception& e) {
		CROW_LOG_ERROR << "PQXX execption: " << e.what();
		handle_error(res, "invalid id", 400);
	}
	catch (const std::exception& e)
	{
		CROW_LOG_ERROR << "Error in get_notification_service_by_id controller: " << e.what();
		handle_error(res, "internal server errror", 500);
	}

}

void NotificationController::handle_notification(pqxx::connection& db, crow::request& req, crow::response& res, const std::string& notification_id) {
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
		if (!(std::string(action) == NotificationServicesStatus::ACCEPTED || std::string(action) == NotificationServicesStatus::REFUSED)) {
			handle_error(res, "action not valid value", 400);
			return;
		}

		//? check if the notificaction exists
		std::unique_ptr<NotificationServiceModel> notification = NotificationServiceModel::get_notification_by_id(db, notification_id);

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

		std::unique_ptr<NotificationServiceModel> updated_notification;

		if (action == NotificationServicesStatus::REFUSED) {
			updated_notification = NotificationServiceModel::handle_notification_status(db, NotificationServicesStatus::REFUSED, notification_id, true);
		}
		else {
			std::unique_ptr<UserModel> notification_creator = UserModel::get_user_by_id(db, notification.get()->get_sender_id());
			std::unique_ptr<UserModel> service_creator = UserModel::get_user_by_id(db, service.get()->get_creator_id());

			if (service.get()->get_type() == ServiceType::OFFERED) {
				if (notification_creator.get()->get_balance() < service.get()->get_price()) {
					handle_error(res, "notification sender does not have enough coins to pay for the service", 400);
					return;
				}
				else {
					int new_sender_balance = notification_creator.get()->get_balance() - service.get()->get_price();

					std::map<std::string, std::string> sender_update_data = {
						{"username", notification_creator.get()->get_username()},
						{"balance", std::to_string(new_sender_balance)}
					};

					UserModel::update_user_by_id(db, notification_creator.get()->get_id(), sender_update_data, true);

					int new_creator_balance = service_creator.get()->get_balance() + service.get()->get_price();

					std::map<std::string, std::string> creator_update_data = {
						{"username", service_creator.get()->get_username()},
						{"balance", std::to_string(new_creator_balance)}
					};

					UserModel::update_user_by_id(db, service_creator.get()->get_id(), creator_update_data, true);
				}
			}
			else {
				if (service_creator.get()->get_balance() < service.get()->get_price()) {
					handle_error(res, "you don't have the coins to pay for this request", 400);
					return;
				}
				else {
					int new_sender_balance = notification_creator.get()->get_balance() + service.get()->get_price();

					std::map<std::string, std::string> sender_update_data = {
						{"username", notification_creator.get()->get_username()},
						{"balance", std::to_string(new_sender_balance)}
					};

					UserModel::update_user_by_id(db, notification_creator.get()->get_id(), sender_update_data, true);

					int new_creator_balance = service_creator.get()->get_balance() - service.get()->get_price();

					std::map<std::string, std::string> creator_update_data = {
						{"username", service_creator.get()->get_username()},
						{"balance", std::to_string(new_creator_balance)}
					};

					UserModel::update_user_by_id(db, service_creator.get()->get_id(), creator_update_data, true);
				}
			}
			updated_notification = NotificationServiceModel::handle_notification_status(db, NotificationServicesStatus::ACCEPTED, notification_id, true);

			std::string service_id = updated_notification.get()->get_service_id();

			std::map<std::string, std::string> service_update_data = {
				{"status",  ServiceStatus::CLOSED},
				{"buyer_id", updated_notification.get()->get_sender_id()}
			};	 

			ServiceModel::update_service_by_id(db, service_id, service_update_data, true);

			bool succes_refused = NotificationServiceModel::refused_notifications(db, updated_notification.get()->get_service_id(), notification_id);

			if (!succes_refused) {
				handle_error(res, "error in refused other notifications", 400);
				return;
			}
			//* Achievements to handle
			std::vector<std::string> vec = {
				AchievementsTitles::ACHIEVEMENT_FOUR, AchievementsTitles::ACHIEVEMENT_FIVE };

			set_new_body_prop(req, "tags", vec);
			set_new_body_prop(req, "primary_user_id", request_id);
			set_new_body_prop(req, "secondary_user_id", updated_notification.get()->get_sender_id());
		}

		//? if action == refused -> refuse the notification
		res.code = 200;
		crow::json::wvalue notification_service_json;
		notification_service_json["id"] = updated_notification.get()->get_id();
		notification_service_json["status"] = updated_notification.get()->get_status();
		notification_service_json["sender_id"] = updated_notification.get()->get_sender_id();
		notification_service_json["service_id"] = updated_notification.get()->get_service_id();

		res.write(notification_service_json.dump());

		res.end();
	}
	catch (const std::exception& e) {
		CROW_LOG_ERROR << "Error in handle_notification controller: " << e.what();
		handle_error(res, "internal server errror", 500);
	}
}

void NotificationController::get_notifications(pqxx::connection& db, const crow::request& req, crow::response& res) {
	try
	{
		crow::json::rvalue body = crow::json::load(req.body);
		std::string request_id = body["id"].s();

		std::vector<NotificationModel> notifications = NotificationModel::get_notifications_self(db, request_id);

		crow::json::wvalue::list notifications_json;

		for (unsigned int i = 0; i < notifications.size(); i++) {
			crow::json::wvalue notification;
			notification["id"] = notifications[i].get_id();
			notification["type"] = notifications[i].get_type();
			notification["created_at"] = notifications[i].get_created_at();
			notification["updated_at"] = notifications[i].get_updated_at();

			if (notifications[i].get_notification_service().has_value()) {
				crow::json::wvalue service_notification_json;
				NotificationServiceModel service_notification = notifications[i].get_notification_service().value();
				service_notification_json["id"] = service_notification.get_id();
				service_notification_json["status"] = service_notification.get_status();
				service_notification_json["service_id"] = service_notification.get_service_id();
				service_notification_json["sender_id"] = service_notification.get_sender_id();
				service_notification_json["notification_id"] = service_notification.get_notification_id();

				std::unique_ptr<ServiceModel> service = ServiceModel::get_service_by_id(db, service_notification.get_service_id(), true);

				crow::json::wvalue service_json;

				service_json["id"] = service.get()->get_id();
				service_json["title"] = service.get()->get_title();
				service_json["description"] = service.get()->get_description();
				service_json["price"] = service.get()->get_price();
				service_json["status"] = service.get()->get_status();
				service_json["type"] = service.get()->get_type();
				if (service.get()->get_image_url().has_value())
					service_json["image_url"] = service.get()->get_image_url().value();
				service_json["created_at"] = service.get()->get_created_at();
				service_json["updated_at"] = service.get()->get_updated_at();

				service_notification_json["service"] = crow::json::wvalue(service_json);

				std::unique_ptr<UserModel> sender = UserModel::get_user_by_id(db, service_notification.get_sender_id(), true);
				crow::json::wvalue sender_json;
				sender_json["id"] = sender.get()->get_id();
				sender_json["community_id"] = sender.get()->get_community_id();
				sender_json["username"] = sender.get()->get_username();
				sender_json["type"] = sender.get()->get_type();
				sender_json["email"] = sender.get()->get_email();
				sender_json["balance"] = sender.get()->get_balance();
				sender_json["created_at"] = sender.get()->get_created_at();
				sender_json["updated_at"] = sender.get()->get_updated_at();

				service_notification_json["sender"] = crow::json::wvalue(sender_json);

				notification["service_notification"] = crow::json::wvalue(service_notification_json);
			}

			notifications_json.push_back(notification);

		}

		res.code = 200;

		crow::json::wvalue data{ {"notifications", notifications_json} };

		res.write(data.dump());

		res.end();
	}
	catch (const pqxx::data_exception& e) {
		CROW_LOG_ERROR << "PQXX execption: " << e.what();
		handle_error(res, "invalid id", 400);
	}
	catch (const std::exception& e)
	{
		CROW_LOG_ERROR << "Error in get_notification_service_by_id controller: " << e.what();
		handle_error(res, "internal server errror", 500);
	}
}
