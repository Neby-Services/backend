#pragma once

#include <utils/common.h>
#include <utils/custom_regex.h>
#include <utils/utils.h>
#include <utils/validations.h>

#include <string>

#include "crow.h"

bool is_correct_body_register(const crow::request &req, crow::response &res);

bool is_correct_body_login(const crow::request &req, crow::response &res);

bool validate_email(const std::string &email, crow::response &res);

bool validate_type(const std::string &type, crow::response &res);

bool validate_password(const std::string &password, crow::response &res);

bool validate_username(const std::string &username, crow::response &res);
