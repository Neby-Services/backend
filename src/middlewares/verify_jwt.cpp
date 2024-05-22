#include <middlewares/verify_jwt.h>

void VerifyJWT::before_handle(crow::request& req, crow::response& res, context& ctx) {
	ConnectionPool& pool = ConnectionPool::getInstance(connection_string, 100);
	auto conn = pool.getConnection();
	std::string token = get_token_cookie(req);

	if (token == "") {
		handle_error(res, "no token provided", 404);
		return;
	}

	if (!validate_token(token)) {
		handle_error(res, "invalid token", 401);
		return;
	}

	auto decoded = jwt::decode(token);

	std::string id;

	for (auto& e : decoded.get_payload_json()) {
		if (e.first == "id")
			id = e.second.get<std::string>();
	}

	std::unique_ptr<UserModel> user = UserModel::get_user_by_id(*conn.get(), id);

	if (!user) {
		handle_error(res, "no authorizated", 403);
		return;
	}

	pool.releaseConnection(conn);

	if (req.body == "") {
		crow::json::wvalue body;

		body["id"] = id;
		body["isAdmin"] = (user.get()->get_type() == "admin");
		body["request_community_id"] = user.get()->get_community_id();
		body["request_username"] = user.get()->get_username();
		body["request_email"] = user.get()->get_email();
		body["request_created_at"] = user.get()->get_created_at();
		body["request_updated_at"] = user.get()->get_updated_at();
		body["request_balance"] = user.get()->get_balance();

		req.body = body.dump();
	}
	else {
		crow::json::wvalue body = crow::json::load(req.body);

		body["id"] = id;
		body["isAdmin"] = (user.get()->get_type() == "admin");
		body["request_community_id"] = user.get()->get_community_id();
		body["request_username"] = user.get()->get_username();
		body["request_email"] = user.get()->get_email();
		body["request_created_at"] = user.get()->get_created_at();
		body["request_updated_at"] = user.get()->get_updated_at();
		body["request_balance"] = user.get()->get_balance();
		req.body = body.dump();
	}
}

void VerifyJWT::after_handle(crow::request& req, crow::response& res, context& ctx) {
}
