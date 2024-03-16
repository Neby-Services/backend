#include "routes/auth_routes.h"

#include "middlewares/user_validation_middleware.h"

void initialize_auth_routes(NebyApp& app, pqxx::connection& db) {
	CROW_ROUTE(app, "/api/auth/register")
		.methods(crow::HTTPMethod::POST)
		.CROW_MIDDLEWARES(app, SignupValidation)([&db](const crow::request& req, crow::response& res) {
			AuthController::register_user(db, req, res);
		});
}
