#include <routes/service_routes.h>

void initialize_service_routes(NebyApp& app, pqxx::connection& db) {
	// ** GET /api/services

	CROW_ROUTE(app, "/api/services").methods(crow::HTTPMethod::GET).CROW_MIDDLEWARES(app, VerifyJWT)([&db](const crow::request& req, crow::response& res) {
		ServiceController::get_services(db, req, res);
	});

	// ** GET /api/services/:id

	// ** POST /api/services

	CROW_ROUTE(app, "/api/services").methods(crow::HTTPMethod::POST).CROW_MIDDLEWARES(app, VerifyJWT)([&db](const crow::request& req, crow::response& res) {
		ServiceController::create_service(db, req, res);
	});

	// ** PUT /api/services/:id

	// ** DELETE /api/services/:id
}