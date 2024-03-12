#include "routes/user_routes.h"

void initialize_user_routes(NebyApp& app, pqxx::connection& db) {
	CROW_ROUTE(app, "/api/users").methods("GET"_method)([&db](const crow::request& req, crow::response& res) {
		UserController::get_users(db, req, res);
	});
}
