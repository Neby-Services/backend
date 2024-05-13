#pragma once

#include <crow.h>
#include <jwt-cpp/jwt.h>
#include <models/user_model.h>
#include <memory>
#include <string>

std::string create_token(std::unique_ptr<UserModel>& user);

bool validate_token(const std::string& token);

std::string get_token_cookie(crow::request& req);
