#include <routes/auth_routes.h>

void initialize_auth_routes(NebyApp& app, ConnectionPool& pool) {
	CROW_ROUTE(app, "/api/auth/register")
		.methods(crow::HTTPMethod::POST)([&pool](const crow::request& req, crow::response& res) {
			auto conn = pool.get_connection();
			AuthController::register_user(*conn.get(), req, res);
			pool.release_connection(conn);
		});

	CROW_ROUTE(app, "/api/auth/login")
		.methods(crow::HTTPMethod::POST)([&pool](const crow::request& req, crow::response& res) {
			auto conn = pool.get_connection();
			AuthController::login_user(*conn.get(), req, res);
			pool.release_connection(conn);
		});

	CROW_ROUTE(app, "/api/auth/self")
		.methods(crow::HTTPMethod::GET)
		.CROW_MIDDLEWARES(app, VerifyJWT)([&pool](const crow::request& req, crow::response& res) {
			auto conn = pool.get_connection();
			AuthController::get_self(*conn.get(), req, res);
			pool.release_connection(conn);
		});
}
