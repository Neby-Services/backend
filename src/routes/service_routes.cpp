#include <routes/service_routes.h>

void initialize_service_routes(NebyApp& app, ConnectionPool& pool) {
	// ** GET /api/services

	CROW_ROUTE(app, "/api/services").methods(crow::HTTPMethod::GET).CROW_MIDDLEWARES(app, VerifyJWT)([&pool](const crow::request& req, crow::response& res) {
		auto conn = pool.get_connection();
		ServiceController::get_services(*conn.get(), req, res);
		pool.release_connection(conn);
	});

	CROW_ROUTE(app, "/api/services/self").methods(crow::HTTPMethod::GET).CROW_MIDDLEWARES(app, VerifyJWT)([&pool](const crow::request& req, crow::response& res) {
		auto conn = pool.get_connection();
		ServiceController::get_services_self(*conn.get(), req, res);
		pool.release_connection(conn);
	});

	// ** GET /api/services/:id
	CROW_ROUTE(app, "/api/services/<string>").methods(crow::HTTPMethod::GET).CROW_MIDDLEWARES(app, VerifyJWT)([&pool](const crow::request& req, crow::response& res, const std::string& service_id) {
		auto conn = pool.get_connection();
		ServiceController::get_service_by_id(*conn.get(), req, res, service_id);
		pool.release_connection(conn);
	});

	// ** POST /api/services
	CROW_ROUTE(app, "/api/services").methods(crow::HTTPMethod::POST).CROW_MIDDLEWARES(app, VerifyJWT, HandleAchievements)([&pool](crow::request& req, crow::response& res) {
		auto conn = pool.get_connection();
		ServiceController::create_service(*conn.get(), req, res);
		pool.release_connection(conn);
	});

	// ** DELETE /api/services/:id
	CROW_ROUTE(app, "/api/services/<string>").methods(crow::HTTPMethod::DELETE).CROW_MIDDLEWARES(app, VerifyJWT)([&pool](const crow::request& req, crow::response& res, const std::string& service_id) {
		auto conn = pool.get_connection();
		ServiceController::delete_service(*conn.get(), req, res, service_id);
		pool.release_connection(conn);
	});

	// ** PUT /api/services/:id
	CROW_ROUTE(app, "/api/services/<string>").methods(crow::HTTPMethod::PUT).CROW_MIDDLEWARES(app, VerifyJWT)([&pool](const crow::request& req, crow::response& res, const std::string& service_id) {
		auto conn = pool.get_connection();
		ServiceController::update_service(*conn.get(), req, res, service_id);
		pool.release_connection(conn);
	});
}
