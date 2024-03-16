#pragma once

#include <jwt-cpp/jwt.h>

#include <string>

std::string create_token(const std::string& userId);
bool validate_token(const std::string& token);
