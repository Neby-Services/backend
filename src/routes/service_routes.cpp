#include <routes/service_routes.h>

void initialize_service_routes(NebyApp& app, pqxx::connection& db) {
	// ** GET /api/services

	// ** GET /api/services/:id

	// ** POST /api/services

	CROW_ROUTE(app, "/api/services").methods(crow::HTTPMethod::POST).CROW_MIDDLEWARES(app, VerifyJWT)([&db](const crow::request& req, crow::response& res) {
		ServiceController::create_service(db, req, res);
		});

	// ** PUT /api/services/:id

	// ** DELETE /api/services/:id
}