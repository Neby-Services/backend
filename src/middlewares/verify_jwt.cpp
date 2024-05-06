#include <middlewares/verify_jwt.h>
void VerifyJWT::before_handle(crow::request& req, crow::response& res, context& ctx) {
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

	std::string id, type, community_id, username, email, created_at, updated_at, balance;

	for (auto& e : decoded.get_payload_json()) {
		if (e.first == "id") {
			id = e.second.get<std::string>();
		} else if (e.first == "type") {
			type = e.second.get<std::string>();
		} else if (e.first == "community_id") {
			community_id = e.second.get<std::string>();
		} else if (e.first == "username") {
			username = e.second.get<std::string>();
		} else if (e.first == "email") {
			email = e.second.get<std::string>();
		} else if (e.first == "created_at") {
			created_at = e.second.get<std::string>();
		} else if (e.first == "updated_at") {
			updated_at = e.second.get<std::string>();
		} else if (e.first == "balance") {
			balance = e.second.get<std::string>();
		}
	}

	if (req.body == "") {
		crow::json::wvalue body;

		body["id"] = id;
		body["isAdmin"] = (type == "admin");
		body["request_community_id"] = community_id;
		body["request_username"] = username;
		body["request_email"] = email;
		body["request_created_at"] = created_at;
		body["request_updated_at"] = updated_at;
		body["request_balance"] = std::stoi(balance);

		req.body = body.dump();
	} else {
		crow::json::wvalue body = crow::json::load(req.body);

		body["id"] = id;
		body["isAdmin"] = (type == "admin");
		body["request_community_id"] = community_id;
		body["request_username"] = username;
		body["request_email"] = email;
		body["request_created_at"] = created_at;
		body["request_updated_at"] = updated_at;
		body["request_balance"] = std::stoi(balance);
		req.body = body.dump();
	}
}

void VerifyJWT::after_handle(crow::request& req, crow::response& res, context& ctx) {
}
