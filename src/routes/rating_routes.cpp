#include <routes/rating_routes.h>

void initialize_rating_routes(NebyApp& app, ConnectionPool& pool) {
	CROW_ROUTE(app, "/api/ratings/<string>").methods(crow::HTTPMethod::POST).CROW_MIDDLEWARES(app, VerifyJWT)([&pool](const crow::request& req, crow::response& res, const std::string& service_id) {
		auto conn = pool.get_connection();
		RatingController::create_rating(*conn.get(), req, res, service_id);
		pool.release_connection(conn);
	});

	CROW_ROUTE(app, "/api/ratings/user/<string>").methods(crow::HTTPMethod::GET)([&pool](const crow::request& req, crow::response& res, const std::string& user_id) {
		auto conn = pool.get_connection();
		RatingController::get_user_ratings(*conn.get(), res, user_id);
		pool.release_connection(conn);
	});
}
