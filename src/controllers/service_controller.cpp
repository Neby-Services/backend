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

		std::unique_ptr<ServiceModel> service = ServiceModel::create_service(db, user.get()->get_community_id(), creator_id, title, description, price, type, nullptr);

		if (!service) {
			handle_error(res, "internal server error", 500);
			return;
		}

		crow::json::wvalue data(
			{{"id", service.get()->get_id()},
			 {"community_id", service.get()->get_community_id()},
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
		std::vector<std::unique_ptr<ServiceModel>> all_services = ServiceModel::get_open_services_by_community_id(db, user.get()->get_community_id());

		crow::json::wvalue::list services;
		for (unsigned int i = 0; i < all_services.size(); ++i) {
			crow::json::wvalue service;

			service["id"] = all_services[i].get()->get_id();
			service["community_id"] = all_services[i].get()->get_community_id();
			service["creator_id"] = all_services[i].get()->get_creator_id();
			if (all_services[i].get()->get_buyer_id().has_value())
				service["buyer_id"] = all_services[i].get()->get_buyer_id().value();
			service["title"] = all_services[i].get()->get_title();
			service["description"] = all_services[i].get()->get_description();
			service["price"] = all_services[i].get()->get_price();
			service["status"] = all_services[i].get()->get_status();
			service["type"] = all_services[i].get()->get_type();
			if (all_services[i].get()->get_image_url().has_value())
				service["image_url"] = all_services[i].get()->get_image_url().value();
			service["created_at"] = all_services[i].get()->get_created_at();
			service["updated_at"] = all_services[i].get()->get_updated_at();

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
