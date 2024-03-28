#pragma once

#include <utils/errors.h>
#include <memory>
#include <pqxx/pqxx>
#include <string>
#include <vector>

class ServiceModel {
	private:
	std::string _id;
	std::string _creator_id;
	std::string _title;
	std::string _description;
	int _price;
	std::string _status;
	std::string _type;
	std::string _buyer_user_id;

	public:
	std::string get_id();
	std::string get_creator_id();
	std::string get_title();
	std::string get_description();
	int get_price();
	std::string get_status();
	std::string get_type();
	std::string get_buyer_user_id();

	ServiceModel(std::string id, std::string creator_id, std::string title, std::string description, int price, std::string status, std::string type, std::string buyer_user_id = "");

	static std::unique_ptr<ServiceModel> create_service(pqxx::connection& db, std::string creator_id, std::string title, std::string description, int price, std::string type, bool isThrow = false);

	static std::vector<ServiceModel> get_services(pqxx::connection& db, std::string status = "");

	// * static ServiceModel create_notification(pqxx::connection &db, std::string creator_id, std::string title, std::string description, int price);
};
