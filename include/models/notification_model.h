#pragma once

#include <models/achievement_notification_model.h>
#include <models/notification_service_model.h>
#include <utils/common.h>
#include <utils/errors.h>
#include <memory>
#include <optional>
#include <vector>

class NotificationModel {
	private:
	std::string _id;
	std::string _type;
	std::string _created_at;
	std::string _updated_at;
	std::optional<NotificationServiceModel> _notification_service;
	std::optional<AchievementNotificationModel> _achievement_notification;

	public:
	NotificationModel(std::string id, std::string type, std::string created_at, std::string updated_at, std::optional<NotificationServiceModel> notification_service, std::optional<AchievementNotificationModel> achievement_notification);

	std::string get_id() const;
	std::string get_type() const;
	std::string get_created_at() const;
	std::string get_updated_at() const;
	std::optional<NotificationServiceModel> get_notification_service();
	std::optional<AchievementNotificationModel> get_achievement_notification();

	static std::unique_ptr<NotificationModel> create_notification(pqxx::connection& db, const std::string& type, bool throw_when_null = false);
	static std::unique_ptr<NotificationModel> get_notification_service_by_id(pqxx::connection& db, const std::string& sender_id, const std::string& service_id, bool throw_when_null = false);

	static std::vector<NotificationModel> get_notifications_self(pqxx::connection& db, const std::string& user_id, bool throw_when_null = false);
};
