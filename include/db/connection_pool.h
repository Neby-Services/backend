#pragma once

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <pqxx/pqxx>
#include <queue>

class ConnectionPool {
	public:
	ConnectionPool(const std::string& connection_string, int pool_size);
	~ConnectionPool();

	std::shared_ptr<pqxx::connection> get_connection();

	void release_connection(std::shared_ptr<pqxx::connection> conn);

	private:
	std::shared_ptr<pqxx::connection> create_connection();

	private:
	std::string _connection_string;
	int _pool_size;
	std::deque<std::shared_ptr<pqxx::connection>> _connections;
	std::mutex _mutex;
	std::condition_variable _condition;
};
