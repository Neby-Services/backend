#pragma once

#include <iostream>
#include <stdexcept>
#include <string>

class data_not_found_exception : public std::exception {
	private:
	std::string _message;

	public:
	data_not_found_exception(const std::string& msg);
	virtual const char* what() const noexcept override;
};