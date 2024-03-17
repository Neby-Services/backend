#include "utils/user_validations.h"

bool is_correct_body_register(const crow::request &req, crow::response &res) {
	try {
		crow::json::rvalue body = crow::json::load(req.body);

		const std::vector<std::string> required_fields = {"email", "password", "type", "username"};

		if (!validate_required_body_fields(body, required_fields, res)) return false;

		if (!validate_email(body["email"].s(), res) ||
			!validate_type(body["type"].s(), res) ||
			!validate_password(body["password"].s(), res) ||
			!validate_username(body["username"].s(), res)) {
			return false;
		}

		if (body["type"].s() == Roles::ADMIN) {
			const std::vector<std::string> required_field = {"community_name"};
			if (!validate_required_body_fields(body, required_field, res)) {
				handle_error(res, "missing community_name", 404);
				return false;
			}
		} else if (body["type"] == Roles::NEIGHBOR) {
			const std::vector<std::string> required_field = {"community_code"};
			if (!validate_required_body_fields(body, required_field, res)) {
				handle_error(res, "missing community_code", 404);
				return false;
			}
		}

		return true;

	} catch (const std::exception &e) {
		handle_exception(res, "register validation error");
		return false;
	}
}

bool validate_email(const std::string &email, crow::response &res) {
	if (!is_correct_email(email)) {
		handle_error(res, "incorrect email", 400);
		return false;
	}
	return true;
}

bool validate_type(const std::string &type, crow::response &res) {
	if (!is_correct_type(type)) {
		handle_error(res, "incorrect type user", 400);
		return false;
	}
	return true;
}

bool validate_password(const std::string &password, crow::response &res) {
	if (!is_correct_password(password)) {
		handle_error(res, "incorrect password", 400);
		return false;
	}
	return true;
}

bool validate_username(const std::string &username, crow::response &res) {
	if (!is_correct_username(username)) {
		handle_error(res, "incorrect username", 400);
		return false;
	}
	return true;
}
