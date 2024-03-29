#pragma once

#include <crow.h>
#include <jwt-cpp/jwt.h>
#include <utils/auth.h>
#include <utils/utils.h>

struct VerifyJWT : crow::ILocalMiddleware {
	struct context {};

	void before_handle(crow::request& req, crow::response& res, context& ctx) {
		std::string token = get_token_cookie(req);

		if (!validate_token(token)) {
			handle_error(res, "invalid token", 401);
			return;
		}

		auto decoded = jwt::decode(token);

		std::string id;
		std::string type;

		// Acceder al payload del token decodificado
		for (auto& e : decoded.get_payload_json()) {
			if (e.first == "id") {
				id = e.second.get<std::string>();
			} else if (e.first == "type") {
				type = e.second.get<std::string>();
			}
		}

		if (req.body == "") {
			crow::json::wvalue body;

			body["id"] = id;
			body["isAdmin"] = (type == "admin");

			req.body = body.dump();
		} else {
			crow::json::wvalue body = crow::json::load(req.body);

			body["id"] = id;
			body["isAdmin"] = (type == "admin");

			req.body = body.dump();
		}
	}

	void after_handle(crow::request& req, crow::response& res, context& ctx) {}
};
