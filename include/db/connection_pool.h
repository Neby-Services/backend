#pragma once

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <pqxx/pqxx>
#include <queue>

class ConnectionPool {
	public:
	ConnectionPool(const std::string& connectionString, int poolSize);
	~ConnectionPool() ;

	std::shared_ptr<pqxx::connection> getConnection();

	void releaseConnection(std::shared_ptr<pqxx::connection> conn);

	private:

	
	std::shared_ptr<pqxx::connection> createConnection();

	private:
	std::string connectionString_;
	int poolSize_;
	std::deque<std::shared_ptr<pqxx::connection>> connections_;
	std::mutex mutex_;
	std::condition_variable condition_;
};
