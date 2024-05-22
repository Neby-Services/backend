#include <routes/service_routes.h>

void initialize_service_routes(NebyApp& app) {
	// ** GET /api/services

	ConnectionPool& pool = ConnectionPool::getInstance(connection_string, 100);

	CROW_ROUTE(app, "/api/services").methods(crow::HTTPMethod::GET).CROW_MIDDLEWARES(app, VerifyJWT)([&pool](const crow::request& req, crow::response& res) {
		auto conn = pool.getConnection();
		ServiceController::get_services(*conn.get(), req, res);
		pool.releaseConnection(conn);
		});

	CROW_ROUTE(app, "/api/services/self").methods(crow::HTTPMethod::GET).CROW_MIDDLEWARES(app, VerifyJWT)([&pool](const crow::request& req, crow::response& res) {
		auto conn = pool.getConnection();
		ServiceController::get_services_self(*conn.get(), req, res);
		pool.releaseConnection(conn);
		});

	// ** GET /api/services/:id
	CROW_ROUTE(app, "/api/services/<string>").methods(crow::HTTPMethod::GET).CROW_MIDDLEWARES(app, VerifyJWT)([&pool](const crow::request& req, crow::response& res, const std::string& service_id) {
		auto conn = pool.getConnection();
		ServiceController::get_service_by_id(*conn.get(), req, res, service_id);
		pool.releaseConnection(conn);
		});

	// ** POST /api/services 
	CROW_ROUTE(app, "/api/services").methods(crow::HTTPMethod::POST).CROW_MIDDLEWARES(app, VerifyJWT, HandleAchievements)([&pool](crow::request& req, crow::response& res) {
		auto conn = pool.getConnection();
		ServiceController::create_service(*conn.get(), req, res);
		pool.releaseConnection(conn);
		});


	// ** DELETE /api/services/:id
	CROW_ROUTE(app, "/api/services/<string>").methods(crow::HTTPMethod::DELETE).CROW_MIDDLEWARES(app, VerifyJWT)([&pool](const crow::request& req, crow::response& res, const std::string& service_id) {
		auto conn = pool.getConnection();
		ServiceController::delete_service(*conn.get(), req, res, service_id);
		pool.releaseConnection(conn);
		});

	// ** PUT /api/services/:id
	CROW_ROUTE(app, "/api/services/<string>").methods(crow::HTTPMethod::PUT).CROW_MIDDLEWARES(app, VerifyJWT)([&pool](const crow::request& req, crow::response& res, const std::string& service_id) {
		auto conn = pool.getConnection();
		ServiceController::update_service(*conn.get(), req, res, service_id);
		pool.releaseConnection(conn);
		});
}
