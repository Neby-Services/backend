#pragma once

#include <crow.h>
#include <string>
#include <vector>

void handle_exception(crow::response &res, const std::string &error_message);

void handle_error(crow::response &res, const std::string &error_message, const int &status_code);

void set_new_body_prop(crow::request &req, const std::string& key, const std::vector<std::string> &list);

void set_new_body_prop(crow::request &req, const std::string &key, const std::string &value);
