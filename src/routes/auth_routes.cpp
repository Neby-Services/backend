#include "auth_routes.h"

void initialize_auth_routes(crow::SimpleApp& app, pqxx::connection& db) {
	CROW_ROUTE(app, "/api/auth/register")
	([&db](const crow::request& req, crow::response& res) {
		res.code = 200;
		res.body = "Register";
		res.end();
	});
}
