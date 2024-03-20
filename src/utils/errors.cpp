#include <utils/errors.h>

data_not_found_exception::data_not_found_exception(const std::string& msg) : _message(msg) {}

const char* data_not_found_exception::what() const noexcept  {
	return _message.c_str();
}

