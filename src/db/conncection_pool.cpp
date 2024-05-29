#include <db/connection_pool.h>

std::shared_ptr<pqxx::connection> ConnectionPool::create_connection() {
	auto conn = std::make_shared<pqxx::connection>(_connection_string);
	if (!conn->is_open()) {
		std::cerr << "Error in connection: " << _connection_string << std::endl;
		exit(1);
	}
	std::cout << "Connection established. " << std::endl;
	return conn;
}

std::shared_ptr<pqxx::connection> ConnectionPool::get_connection() {
	std::unique_lock<std::mutex> lock(_mutex);

	while (_connections.empty()) {
		_condition.wait(lock);
	}

	auto conn = _connections.front();
	_connections.pop_front();

	return conn;
}

void ConnectionPool::release_connection(std::shared_ptr<pqxx::connection> conn) {
	std::unique_lock<std::mutex> lock(_mutex);
	_connections.push_back(conn);

	_condition.notify_one();
}

ConnectionPool::ConnectionPool(const std::string& connection_string, int pool_size)
	: _connection_string(connection_string), _pool_size(pool_size) {
	for (int i = 0; i < _pool_size; ++i) {
		_connections.push_back(create_connection());
	}
}

ConnectionPool::~ConnectionPool() {
	for (auto& conn : _connections) {
		conn->disconnect();
	}
}
