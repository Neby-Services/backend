#include <controllers/service_controller.h>

void ServiceController::create_service(pqxx::connection &db, const crow::request &req, crow::response &res) {
	try {
		crow::json::rvalue body = crow::json::load(req.body);

		std::string service_creator_id = body["id"].s();

		std::unique_ptr<ServiceModel> service = ServiceModel::create_service(db, service_creator_id, "service_1", "description_1", 20);

		if (!service) {
			handle_error(res, "service error controller", 400);
			return;
		}

		res.code = 201;
		res.body = "todo ok";
		res.end();
	} catch (const pqxx::data_exception &e) {
		handle_error(res, "invalid properties to create service", 400);

	} catch (const std::exception &e) {
		handle_error(res, e.what(), 500);
	}
}