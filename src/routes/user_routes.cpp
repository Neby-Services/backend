#include "routes/user_routes.h"

void initialize_user_routes(crow::SimpleApp& app, pqxx::connection& db) {
	CROW_ROUTE(app, "/api/users").methods("GET"_method)([&db](const crow::request& req, crow::response& res) {
		// UserController::get_all_users(db, req, res);
		res.code = 200;
		res.body = TestController::index(db);
		res.end();
	});
}
