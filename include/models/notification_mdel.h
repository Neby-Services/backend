#pragma once

#include <utils/common.h>

class NotificationModel {
	private:
	std::string _id;
	std::string _sender_id;
	std::string _receiver_id;
	std::string _service_id;
	std::string _status;
	std::string _created_at;
	std::string _updated_at;

	public:
	NotificationModel(std::string id, std::string sender_id, std::string receiver_id, std::string service_id, std::string status);

    std::string get_id();
    std::string get_sender_id();
    std::string get_receiver_id();
    std::string get_service_id();
    std::string get_status();

};

/* CREATE TABLE IF NOT EXISTS notifications (
	id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
	sender_id UUID REFERENCES users(id),
	receiver_id UUID REFERENCES users(id),
	service_id UUID REFERENCES services(id),
	status notification_status DEFAULT 'PENDING', -- PENDING, ACCEPTED, REFUSED
	created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
	updated_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP
);
 */