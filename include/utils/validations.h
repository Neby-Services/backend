#pragma once

#include <crow.h>

#include <regex>

bool validate_required_body_fields(const crow::json::rvalue &body, const std::vector<std::string> &required_fields, crow::response &res);

bool isValidUUID(const std::string &uuid);