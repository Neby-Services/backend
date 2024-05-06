#pragma once

#include <utils/common.h>

class NotificationModel {
	private:
	std::string _id;
	std::string _sender_id;
	std::string _service_id;
	std::string _status;
	std::string _created_at;
	std::string _updated_at;

	public:
	NotificationModel(std::string id, std::string sender_id, std::string service_id, std::string status);

	std::string get_id() const;
	std::string get_sender_id() const;
	std::string get_service_id() const;
	std::string get_status() const;
};
