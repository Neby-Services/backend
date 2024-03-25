#pragma once

#include <models/user_model.h>
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
	UserModel _creator;
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

	ServiceModel(std::string id, std::string creator_id, std::string title, std::string description, int price, std::string status, std::string type, UserModel creator, std::string buyer_user_id = "");

	static std::unique_ptr<ServiceModel> create_service(pqxx::connection& db, std::string creator_id, std::string title, std::string description, int price, std::string type, bool isThrow = false);

	static std::vector<ServiceModel> get_services(pqxx::connection& db, std::string status = "");

	// * static ServiceModel create_notification(pqxx::connection &db, std::string creator_id, std::string title, std::string description, int price);
};

/*
CREATE TABLE IF NOT EXISTS services (
	id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
	creator_id UUID REFERENCES users(id),
	service_name VARCHAR(255) NOT NULL,
	description TEXT,
	price NUMERIC(10, 2),
	status service_status DEFAULT 'OPEN', -- Estado del servicio: 'OPEN' o 'CLOSED'
	usuario_venta_id UUID,
	created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
	updated_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP
); */
