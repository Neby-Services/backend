#include <routes/auth_routes.h>

void initialize_auth_routes(NebyApp& app) {

	ConnectionPool& pool = ConnectionPool::getInstance(connection_string, 10);

	CROW_ROUTE(app, "/api/auth/register")
		.methods(crow::HTTPMethod::POST)([&pool](const crow::request& req, crow::response& res) {
			auto conn = pool.getConnection();
			AuthController::register_user(*conn.get(), req, res);
			pool.releaseConnection(conn);
		});

	CROW_ROUTE(app, "/api/auth/login")
		.methods(crow::HTTPMethod::POST)([&pool](const crow::request& req, crow::response& res) {
			auto conn = pool.getConnection();
			std::cout << "me mamais los putisimos huevosss" << std::endl; 
			AuthController::login_user(*conn.get(), req, res);
			pool.releaseConnection(conn);
		});

	CROW_ROUTE(app, "/api/auth/self")
		.methods(crow::HTTPMethod::GET)
		.CROW_MIDDLEWARES(app, VerifyJWT)([&pool](const crow::request& req, crow::response& res) {
			auto conn = pool.getConnection();
			AuthController::get_self(*conn.get(), req, res);
			pool.releaseConnection(conn);
		});
}
