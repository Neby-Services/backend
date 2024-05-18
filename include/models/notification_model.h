#pragma once

#include <utils/common.h>
#include <utils/errors.h>
#include <memory>
#include <vector>

class NotificationModel {
	private:
	std::string _id;
	std::string _sender_id;
	std::string _service_id;
	std::string _status;
	std::string _created_at;
	std::string _updated_at;

	public:
	NotificationModel(std::string id, std::string sender_id, std::string service_id, std::string status, std::string created_at, std::string updated_at);

	std::string get_id() const;
	std::string get_sender_id() const;
	std::string get_service_id() const;
	std::string get_status() const;
	std::string get_created_at() const;
	std::string get_updated_at() const;

	static std::unique_ptr<NotificationModel> create_notification(pqxx::connection& db, const std::string& sender_id, const std::string& service_id, const std::string& status = NotificationStatus::PENDING, bool throw_when_null = false);

	// * if the requester has already requested the service before, it returns true, otherwise false
	static bool is_requested(pqxx::connection& db, const std::string& sender_id);

	static std::unique_ptr<NotificationModel> handle_notification_status(pqxx::connection& db, const std::string& status, const std::string& notification_id, bool throw_when_null = false);

	static bool refused_notifications(pqxx::connection& db, const std::string& service_id, const std::string& notification_id);

	static std::unique_ptr<NotificationModel> get_notification_by_id(pqxx::connection& db, const std::string& id, bool throw_when_null = false);

	static std::vector<std::unique_ptr<NotificationModel>> get_notifications_accepted_self(pqxx::connection& db, const std::string& sender_id);
};
