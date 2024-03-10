#include "auth_controller.h"

void UserController::create_user(pqxx::connection& db, const crow::request& req, crow::response& res) {
	res.code = 200;
	crow::json::wvalue x({{"message", "Hello, World!"}});
	x["message2"] = "Hello, World.. Again!";
	res.write(x.dump());
	res.end();
}