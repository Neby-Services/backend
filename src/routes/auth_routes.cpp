#include <routes/auth_routes.h>

void initialize_auth_routes(NebyApp& app, pqxx::connection& db) {
	CROW_ROUTE(app, "/api/auth/register")
		.methods(crow::HTTPMethod::POST)([&db](const crow::request& req, crow::response& res) {
			AuthController::register_user(db, req, res);
		});

	CROW_ROUTE(app, "/api/auth/login")
		.methods(crow::HTTPMethod::POST)([&db](const crow::request& req, crow::response& res) {
			AuthController::login_user(db, req, res);
		});

	CROW_ROUTE(app, "/api/auth/self")
		.methods(crow::HTTPMethod::GET)
		.CROW_MIDDLEWARES(app, VerifyJWT)([&db](const crow::request& req, crow::response& res) {
			AuthController::get_self(db, req, res);
		});
}
