#ifndef TEST_CONTROLLER_H
#define TEST_CONTROLLER_H

#include <string>
#include <format>
#include "../models/test_model.h"

class TestController {
	public:
	static std::string index(pqxx::connection& db);
};

#endif
