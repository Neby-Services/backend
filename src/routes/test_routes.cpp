#include "test_routes.h"

void initialize_test_routes(crow::SimpleApp& app, pqxx::connection& db) {
	CROW_ROUTE(app, "/api/test") ([&db](const crow::request& req, crow::response& res){
		res.code = 200;
		res.body = TestController::index(db);
		res.end();
	});
}
