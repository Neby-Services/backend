#include "utils/user_validations.h"

bool is_correct_body(const crow::request &req, crow::response &res) {
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

		return true;

	} catch (const std::exception &e) {
		handle_exception(res, "register validation error");
		return false;
	}
}

bool validate_email(const std::string &email, crow::response &res) {
	if (!is_correct_email(email)) {
		handle_error(res, "incorrect email");
		return false;
	}
	return true;
}

bool validate_type(const std::string &type, crow::response &res) {
	if (!is_correct_type(type)) {
		handle_error(res, "incorrect type user");
		return false;
	}
	return true;
}

bool validate_password(const std::string &password, crow::response &res) {
	if (!is_correct_password(password)) {
		handle_error(res, "incorrect password");
		return false;
	}
	return true;
}

bool validate_username(const std::string &username, crow::response &res) {
	if (!is_correct_username(username)) {
		handle_error(res, "incorrect username");
		return false;
	}
	return true;
}
