#pragma once

#include <crow.h>

bool validate_required_body_fields(const crow::json::rvalue &body, const std::vector<std::string> &required_fields, crow::response &res);
