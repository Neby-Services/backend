#include "test_model.h"

TestModel::TestModel(std::string time) : time(time) {}

std::string TestModel::getTime() {
	return time;
}

TestModel TestModel::timeNow(pqxx::connection& db) {
	pqxx::work txn(db);
	pqxx::result result = txn.exec("SELECT NOW()");
	std::string res = result[0][0].as<std::string>();
	txn.commit();
	return TestModel(res);
}
