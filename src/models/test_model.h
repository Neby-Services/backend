#ifndef TEST_MODEL_H
#define TEST_MODEL_H

#include <string>
#include <pqxx/pqxx>

class TestModel {
private:
	std::string time;

public:
	TestModel(std::string time);

	std::string getTime();

	static TestModel timeNow(pqxx::connection& db);
};

#endif
