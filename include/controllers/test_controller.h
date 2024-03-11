#include <format>
#include <string>

#include "models/test_model.h"

class TestController {
	public:
	static std::string index(pqxx::connection& db);
};
