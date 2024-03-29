#include <utils/errors.h>

data_not_found_exception::data_not_found_exception(const std::string& msg) : _message(msg) {}

const char* data_not_found_exception::what() const noexcept {
	return _message.c_str();
}

creation_exception::creation_exception(const std::string& msg) : _message(msg) {}

const char* creation_exception::what() const noexcept {
	return _message.c_str();
}

deletion_exception::deletion_exception(const std::string& msg) : _message(msg) {}

const char* deletion_exception::what() const noexcept {
	return _message.c_str();
}

update_exception::update_exception(const std::string& msg) : _message(msg) {}

const char* update_exception::what() const noexcept {
	return _message.c_str();
}
