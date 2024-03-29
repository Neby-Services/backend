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

class creation_exception : public std::exception {
	private:
	std::string _message;

	public:
	creation_exception(const std::string& msg);
	virtual const char* what() const noexcept override;
};

class deletion_exception : public std::exception {
	private:
	std::string _message;

	public:
	deletion_exception(const std::string& msg);
	virtual const char* what() const noexcept override;
};

class update_exception : public std::exception {
	private:
	std::string _message;

	public:
	update_exception(const std::string& msg);
	virtual const char* what() const noexcept override;
};
