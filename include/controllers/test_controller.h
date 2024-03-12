#ifndef TEST_CONTROLLER_h
#define TEST_CONTROLLER_h

#include <format>
#include <string>

#include "models/test_model.h"

class TestController {
	public:
	static std::string index(pqxx::connection& db);
};

#endif