#include <db/connection_pool.h>

std::shared_ptr<pqxx::connection> ConnectionPool::createConnection() {
	auto conn = std::make_shared<pqxx::connection>(connectionString_);
	if (!conn->is_open()) {
		std::cerr << "Error in connection: " << connectionString_ << std::endl;
		exit(1);
	}
	std::cout << "Connection established. " << std::endl;
	return conn;
}

std::shared_ptr<pqxx::connection> ConnectionPool::getConnection() {
	std::unique_lock<std::mutex> lock(mutex_);

	while (connections_.empty()) {
		condition_.wait(lock);
	}

	auto conn = connections_.front();
	connections_.pop_front();

	return conn;
}

void ConnectionPool::releaseConnection(std::shared_ptr<pqxx::connection> conn) {
	std::unique_lock<std::mutex> lock(mutex_);
	connections_.push_back(conn);

	condition_.notify_one();
}

ConnectionPool::ConnectionPool(const std::string& connectionString, int poolSize)
	: connectionString_(connectionString), poolSize_(poolSize) {
	for (int i = 0; i < poolSize_; ++i) {
		connections_.push_back(createConnection());
	}
}

ConnectionPool::~ConnectionPool() {
	for (auto& conn : connections_) {
		conn->disconnect();
	}
}
