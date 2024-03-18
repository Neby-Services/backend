#pragma once

#include <jwt-cpp/jwt.h>

#include <string>

std::string create_token(const std::string& userId, const std::string& type);
bool validate_token(const std::string& token);
