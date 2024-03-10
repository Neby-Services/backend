#include "test_controller.h"

std::string TestController::index(pqxx::connection& db) {
	try {
		TestModel time = TestModel::timeNow(db);
		return std::format("Data from database, actual time: {}", time.getTime());
	} catch (const std::exception& e) {
		return std::format("Error: {}", e.what());
	}
}
