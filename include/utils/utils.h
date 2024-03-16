#pragma once

#include <string>

#include "crow.h"

void handle_exception(crow::response &res, const std::string &error_message);

void handle_error(crow::response &res, const std::string &error_message, const int &status_code);
