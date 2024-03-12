#include <crow.h>

#include <string>

void handle_exception(crow::response &res, const std::string &error_message);

void handle_error(crow::response &res, const std::string &error_message);