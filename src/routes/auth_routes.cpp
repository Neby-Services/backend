#include "auth_routes.h"

void initialize_auth_routes(crow::SimpleApp& app, pqxx::connection& db) {
	CROW_ROUTE(app, "/api/auth/register").methods("POST"_method)([&db](const crow::request& req, crow::response& res) {
		UserController::create_user(db, req, res);
	});
}
