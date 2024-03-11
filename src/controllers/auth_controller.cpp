#include "controllers/auth_controller.h"

void AuthController::register_user(pqxx::connection& db, const crow::request& req, crow::response& res) {
	/* 	res.code = 200;
		auto y = crow::json::load(req.body);

		crow::json::wvalue x({{"message", "Hello, World!"}});
		x["message2"] = "Hello!";
		res.write(x.dump());
		res.end(); */

	auto y = crow::json::load(req.body);
	crow::json::wvalue x(y);

	res.write(x.dump());
	res.end();
}
