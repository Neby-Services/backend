#include <utils/utils.h>

void handle_exception(crow::response &res, const std::string &error_message) {
	res.code = 500;
	crow::json::wvalue error({{"error", error_message}});
	res.write(error.dump());
	res.end();
}

void handle_error(crow::response &res, const std::string &error_message, const int &status_code) {
	crow::json::wvalue error({{"error", error_message}});
	res.code = status_code;
	res.write(error.dump());
	res.end();
}
