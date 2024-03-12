#include <utils/utils.h>

void handle_exception(crow::response &res, const std::string &error_message) {
	res.code = 500;
	crow::json::wvalue error({{"error", error_message}});
	res.write(error.dump());
	res.end();
}

void handle_error(crow::response &res, const std::string &error_message) {
	crow::json::wvalue error({{"error", error_message}});
	res.code = 400;
	res.write(error.dump());
	res.end();
}