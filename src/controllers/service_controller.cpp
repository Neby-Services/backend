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