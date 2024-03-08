#include "test_routes.h"

void initialize_test_routes(crow::SimpleApp& app) {
	CROW_ROUTE(app, "/api/test") ([](const crow::request& req, crow::response& res){
		res.code = 200;
		res.body = TestController::index();
		res.end();
	});
}
