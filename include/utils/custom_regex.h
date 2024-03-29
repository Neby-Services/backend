#pragma once

#include <utils/common.h>
#include <cctype>
#include <iostream>
#include <regex>
#include <set>
#include <string>

bool is_correct_email(const std::string &email);

bool is_correct_type(std::string type);

bool is_correct_password(std::string password);

bool is_correct_username(std::string username);
