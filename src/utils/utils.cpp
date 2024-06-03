#include <utils/utils.h>

void handle_exception(crow::response &res, const std::string &error_message) {
	res.code = 500;
	crow::json::wvalue error({{"error", error_message}});
	res.write(error.dump());
	res.end();
}

void handle_error(crow::response &res, const std::string &error_message, const int &status_code) {
	crow::json::wvalue error({{"error", error_message}});
	res.code = status_code;
	res.write(error.dump());
	res.end();
}

void set_new_body_prop(crow::request &req, const std::string &key, const std::vector<std::string> &list) {
	crow::json::wvalue body = crow::json::load(req.body);
	crow::json::wvalue::list vec_json;
	
	for (const auto &e : list) {
		vec_json.push_back(e);
	}

	body[key] = crow::json::wvalue::list(vec_json);

	req.body = body.dump();
}

void set_new_body_prop(crow::request &req, const std::string &key, const std::string &value) {
	crow::json::wvalue body = crow::json::load(req.body);

	body[key] = value;

	req.body = body.dump();
}
