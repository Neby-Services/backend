#include <routes/user_routes.h>

void initialize_user_routes(NebyApp& app, ConnectionPool& pool) {
	CROW_ROUTE(app, "/api/users").methods(crow::HTTPMethod::GET).CROW_MIDDLEWARES(app, VerifyJWT)([&pool](const crow::request& req, crow::response& res) {
		auto conn = pool.get_connection();
		UserController::get_users(*conn.get(), req, res);
		pool.release_connection(conn);
	});

	CROW_ROUTE(app, "/api/users/self").methods(crow::HTTPMethod::PUT).CROW_MIDDLEWARES(app, VerifyJWT)([&pool](const crow::request& req, crow::response& res) {
		auto conn = pool.get_connection();
		UserController::update_self(*conn.get(), req, res);
		pool.release_connection(conn);
	});

	CROW_ROUTE(app, "/api/users/self").methods(crow::HTTPMethod::DELETE).CROW_MIDDLEWARES(app, VerifyJWT)([&pool](const crow::request& req, crow::response& res) {
		auto conn = pool.get_connection();
		UserController::delete_self(*conn.get(), req, res);
		pool.release_connection(conn);
	});

	CROW_ROUTE(app, "/api/users/<string>").methods(crow::HTTPMethod::GET).CROW_MIDDLEWARES(app, VerifyJWT)([&pool](const crow::request& req, crow::response& res, const std::string& user_id) {
		auto conn = pool.get_connection();
		UserController::get_user_by_id(*conn.get(), req, res, user_id);
		pool.release_connection(conn);
	});

	CROW_ROUTE(app, "/api/users/<string>").methods(crow::HTTPMethod::DELETE).CROW_MIDDLEWARES(app, VerifyJWT)([&pool](const crow::request& req, crow::response& res, const std::string& user_id) {
		auto conn = pool.get_connection();
		UserController::delete_user_by_id(*conn.get(), req, res, user_id);
		pool.release_connection(conn);
	});

	CROW_ROUTE(app, "/api/users/<string>").methods(crow::HTTPMethod::PUT).CROW_MIDDLEWARES(app, VerifyJWT)([&pool](const crow::request& req, crow::response& res, const std::string& user_id) {
		auto conn = pool.get_connection();
		UserController::update_user_by_id(*conn.get(), req, res, user_id);
		pool.release_connection(conn);
	});
}
