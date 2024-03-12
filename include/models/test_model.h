#ifndef TEST_MODEL_h
#define TEST_MODEL_h

#include <pqxx/pqxx>
#include <string>

class TestModel {
	private:
	std::string time;

	public:
	TestModel(std::string time);

	std::string getTime();

	static TestModel timeNow(pqxx::connection& db);
};

#endif
