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

		std::string service_creator_id = body["id"].s();

		std::unique_ptr<ServiceModel> service = ServiceModel::create_service(db, service_creator_id, title, description, price, type);

		if (!service) {
			handle_error(res, "service error controller", 400);
			return;
		}

		crow::json::wvalue data({
			{"type", service.get()->get_type()},
			{"status", service.get()->get_status()},
			{"price", service.get()->get_price()},
			{"creator_id", service.get()->get_creator_id()},
			{"description", service.get()->get_description()},
			{"title", service.get()->get_title()},
			{"id", service.get()->get_id()},
		});

		res.write(data.dump());
		res.code = 201;
		res.end();
	} catch (const pqxx::data_exception &e) {
		handle_error(res, "invalid properties to create service", 400);

	} catch (const std::exception &e) {
		handle_error(res, e.what(), 500);
	}
}

void ServiceController::get_services(pqxx::connection &db, const crow::request &req, crow::response &res) {
	try {
		// ** comment to add 
		auto status_param = req.url_params.get("status");
		std::string status = status_param;
		std::vector<ServiceModel> allServices = ServiceModel::get_services(db, status);
		if (!status_param)
			allServices = ServiceModel::get_services(db);

		crow::json::wvalue::list services;
		for (unsigned int i = 0; i < allServices.size(); i++) {
			crow::json::wvalue service;
			service["id"] = allServices[i].get_id();
			service["creator_id"] = allServices[i].get_creator_id();
			service["title"] = allServices[i].get_title();
			service["description"] = allServices[i].get_description();
			service["price"] = allServices[i].get_price();
			service["status"] = allServices[i].get_status();
			service["type"] = allServices[i].get_type();
			service["buyer_user_id"] = allServices[i].get_buyer_user_id();
			services.push_back(service);
		}

		crow::json::wvalue data{{"services", services}};
		res.write(data.dump());
		res.code = 200;
		res.end();
	} catch (const std::exception &e) {
		handle_error(res, e.what(), 500);
	}
}