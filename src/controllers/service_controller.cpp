#include <controllers/service_controller.h>

void ServiceController::create_service(pqxx::connection &db, const crow::request &req, crow::response &res) {
	try {
		crow::json::rvalue body = crow::json::load(req.body);

		const std::vector<std::string> required_fields = {"title", "description", "price", "id", "type"};

		if (!validate_required_body_fields(body, required_fields, res)) return;

		std::string title = body["title"].s();
		std::string description = body["description"].s();
		int price = body["price"].i();
		std::string type = body["type"].s();
		std::string creator_id = body["id"].s();

		std::unique_ptr<UserModel> user = UserModel::get_user_by_id(db, creator_id);

		std::cout << "id creator -> " << creator_id << std::endl;

		std::unique_ptr<ServiceModel> service = ServiceModel::create_service(db, creator_id, title, description, price, type, std::nullopt);

		if (!service) {
			handle_error(res, "internal server error", 500);
			return;
		}

		crow::json::wvalue data(
			{{"id", service.get()->get_id()},
			 {"creator_id", service.get()->get_creator_id()},
			 {"title", service.get()->get_title()},
			 {"description", service.get()->get_description()},
			 {"price", service.get()->get_price()},
			 {"status", service.get()->get_status()},
			 {"type", service.get()->get_type()},
			 {"created_at", service.get()->get_created_at()},
			 {"updated_at", service.get()->get_updated_at()}});

		if (service.get()->get_buyer_id().has_value())
			data["buyer_id"] = service.get()->get_buyer_id().value();
		if (service.get()->get_image_url().has_value())
			data["image_url"] = service.get()->get_image_url().value();

		res.code = 201;
		res.write(data.dump());
		res.end();
	} catch (const std::exception &e) {
		std::cerr << "Error creating service: " << e.what() << std::endl;
		handle_error(res, "internal server error", 500);
	}
}

void ServiceController::get_services(pqxx::connection &db, const crow::request &req, crow::response &res) {
	try {
		crow::json::rvalue body = crow::json::load(req.body);

		std::unique_ptr<UserModel> user = UserModel::get_user_by_id(db, body["id"].s());
		std::vector<std::unique_ptr<ServiceModel>> all_services;

		auto status = req.url_params.get("status");

		if (!status) {
			all_services = ServiceModel::get_services(db, user.get()->get_community_id());

		} else if (status && (std::string(status) == ServiceStatus::CLOSED || std::string(status) == ServiceStatus::OPEN)) {
			all_services = ServiceModel::get_services(db, user.get()->get_community_id(), status);
		} else {
			handle_error(res, "status not valid value", 400);
			return;
		}

		crow::json::wvalue::list services;
		for (unsigned int i = 0; i < all_services.size(); ++i) {
			crow::json::wvalue service;

			service["id"] = all_services[i].get()->get_id();
			service["creator_id"] = all_services[i].get()->get_creator_id();
			if (all_services[i].get()->get_buyer_id().has_value()) {
				crow::json::wvalue buyer;
				buyer["id"] = all_services[i].get()->get_buyer().get_id();
				buyer["username"] = all_services[i].get()->get_buyer().get_username();
				buyer["type"] = all_services[i].get()->get_buyer().get_type();
				buyer["email"] = all_services[i].get()->get_buyer().get_email();
				buyer["balance"] = all_services[i].get()->get_buyer().get_balance();
				buyer["created_at"] = all_services[i].get()->get_buyer().get_created_at();
				buyer["updated_at"] = all_services[i].get()->get_buyer().get_updated_at();

				service["buyer"] = crow::json::wvalue(buyer);
				service["buyer_id"] = all_services[i].get()->get_buyer_id().value();
			}
			service["title"] = all_services[i].get()->get_title();
			service["description"] = all_services[i].get()->get_description();
			service["price"] = all_services[i].get()->get_price();
			service["status"] = all_services[i].get()->get_status();
			service["type"] = all_services[i].get()->get_type();
			if (all_services[i].get()->get_image_url().has_value())
				service["image_url"] = all_services[i].get()->get_image_url().value();
			service["created_at"] = all_services[i].get()->get_created_at();
			service["updated_at"] = all_services[i].get()->get_updated_at();

			crow::json::wvalue creator;
			creator["id"] = all_services[i].get()->get_creator().get_id();
			creator["username"] = all_services[i].get()->get_creator().get_username();
			creator["type"] = all_services[i].get()->get_creator().get_type();
			creator["email"] = all_services[i].get()->get_creator().get_email();
			creator["balance"] = all_services[i].get()->get_creator().get_balance();
			creator["created_at"] = all_services[i].get()->get_creator().get_created_at();
			creator["updated_at"] = all_services[i].get()->get_creator().get_updated_at();

			service["creator"] = crow::json::wvalue(creator);

			services.push_back(service);
		}

		crow::json::wvalue data{{"services", services}};

		res.write(data.dump());
		res.code = 200;
		res.end();
	} catch (const std::exception &e) {
		std::cerr << "Error getting services: " << e.what() << std::endl;
		handle_error(res, "internal server error", 500);
	}
}

void ServiceController::delete_service(pqxx::connection &db, const crow::request &req, crow::response &res, std::string service_id) {
	try {
		crow::json::rvalue request = crow::json::load(req.body);
		// if (request["isAdmin"].b() == true) {
		std::unique_ptr<ServiceModel> service = ServiceModel::get_service_by_id(db, service_id);

		if (!service) {
			handle_error(res, "service not found", 404);
			return;
		}

		std::string service_creator_id = service.get()->get_creator_id();

		std::unique_ptr<UserModel> creator = UserModel::get_user_by_id(db, service_creator_id);
		std::string service_community = creator.get()->get_community_id();

		std::unique_ptr<UserModel> admin = UserModel::get_user_by_id(db, request["id"].s());
		std::string admin_community = admin.get()->get_community_id();

		if ((service_community == admin_community && request["isAdmin"].b() == true) || service_creator_id == request["id"].s()) {
			std::unique_ptr<ServiceModel> deleted_service = ServiceModel::delete_service_by_id(db, service_id);

			if (deleted_service) {
				res.code = 204;
				res.end();
			} else {
				handle_error(res, "service not found", 404);
				return;
			}
		} else {
			handle_error(res, "user without admin privileges or not creator of service", 403);
			return;
		}

	} catch (const std::exception &e) {
		std::cerr << "Error deleting service: " << e.what() << std::endl;
		handle_error(res, "internal server error", 500);
	}
}