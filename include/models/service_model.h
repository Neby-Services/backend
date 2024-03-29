#pragma once

#include <utils/errors.h>
#include <memory>
#include <optional>
#include <pqxx/pqxx>
#include <string>
#include <vector>

class ServiceModel {
	private:
	std::string _id;
	std::string _community_id;
	std::string _creator_id;
	std::optional<std::string> _buyer_id;
	std::string _title;
	std::string _description;
	int _price;
	std::string _status;
	std::string _type;
	std::optional<std::string> _image_url;
	std::string _created_at;
	std::string _updated_at;

	public:
	ServiceModel(std::string id, std::string community_id, std::string creator_id, std::optional<std::string> buyer_id, std::string title, std::string description, int price, std::string status, std::string type, std::optional<std::string> image_url, std::string created_at, std::string updated_at);

	std::string get_id() const;
	std::string get_community_id() const;
	std::string get_creator_id() const;
	std::optional<std::string> get_buyer_id() const;
	std::string get_title() const;
	std::string get_description() const;
	int get_price() const;
	std::string get_status() const;
	std::string get_type() const;
	std::optional<std::string> get_image_url() const;
	std::string get_created_at() const;
	std::string get_updated_at() const;

	static std::unique_ptr<ServiceModel> create_service(pqxx::connection& db, const std::string& community_id, const std::string& creator_id, const std::string& title, const std::string& description, const int price, const std::string& type, const std::optional<std::string>& image_url, bool isThrow = false);

	static std::vector<std::unique_ptr<ServiceModel>> get_open_services_by_community_id(pqxx::connection& db, const std::string& community_id);
};
