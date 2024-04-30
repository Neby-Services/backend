#include <utils/validations.h>

bool validate_required_body_fields(const crow::json::rvalue &body, const std::vector<std::string> &required_fields, crow::response &res) {
	for (const auto &field : required_fields) {
		if (!body.has(field)) {
			crow::json::wvalue error({{"error", "missing " + field + " field"}});
			res.code = 404;
			res.write(error.dump());
			res.end();
			return false;
		}
	}

	return true;
}

bool isValidUUID(const std::string &uuid) {
	// Regular expression pattern for UUID format with lowercase hexadecimal characters
	std::regex pattern("[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}");

	// Check if the string matches the pattern
	return std::regex_match(uuid, pattern);
}