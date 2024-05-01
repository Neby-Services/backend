#include <routes/service_routes.h>

void initialize_service_routes(NebyApp& app, pqxx::connection& db) {
	// ** GET /api/services

	CROW_ROUTE(app, "/api/services/<string>").methods(crow::HTTPMethod::GET).CROW_MIDDLEWARES(app, VerifyJWT)([&db](const crow::request& req, crow::response& res, const std::string& service_id) {
		ServiceController::get_service_by_id(db, req, res, service_id);
	});

	CROW_ROUTE(app, "/api/services").methods(crow::HTTPMethod::GET).CROW_MIDDLEWARES(app, VerifyJWT)([&db](const crow::request& req, crow::response& res) {
		ServiceController::get_services(db, req, res);
	});

	// ** GET /api/services/:id

	// ** POST /api/services

	CROW_ROUTE(app, "/api/services").methods(crow::HTTPMethod::POST).CROW_MIDDLEWARES(app, VerifyJWT)([&db](const crow::request& req, crow::response& res) {
		ServiceController::create_service(db, req, res);
	});

	// ** PUT /api/services/:id

	CROW_ROUTE(app, "/api/services/<string>").methods(crow::HTTPMethod::PUT).CROW_MIDDLEWARES(app, VerifyJWT)([&db](const crow::request& req, crow::response& res, const std::string& service_id) {
		ServiceController::update_service(db, req, res, service_id);
	});

	// ** DELETE /api/services/:id

	CROW_ROUTE(app, "/api/services/<string>").methods(crow::HTTPMethod::DELETE).CROW_MIDDLEWARES(app, VerifyJWT)([&db](const crow::request& req, crow::response& res, const std::string& service_id) {
		ServiceController::delete_service(db, req, res, service_id);
	});
}
