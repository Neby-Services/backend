#pragma once

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <pqxx/pqxx>
#include <queue>

class ConnectionPool {
	public:
	static ConnectionPool& getInstance(const std::string& connectionString, int poolSize) {
		static ConnectionPool instance(connectionString, poolSize);
		return instance;
	}

	ConnectionPool(const ConnectionPool&) = delete;
	ConnectionPool& operator=(const ConnectionPool&) = delete;

	std::shared_ptr<pqxx::connection> getConnection();

	void releaseConnection(std::shared_ptr<pqxx::connection> conn);

	private:
	ConnectionPool(const std::string& connectionString, int poolSize);

	~ConnectionPool() ;
	
	std::shared_ptr<pqxx::connection> createConnection();

	private:
	std::string connectionString_;
	int poolSize_;
	std::deque<std::shared_ptr<pqxx::connection>> connections_;
	std::mutex mutex_;
	std::condition_variable condition_;
};
