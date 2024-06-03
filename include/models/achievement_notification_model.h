#pragma once

#include <utils/common.h>
#include <utils/errors.h>
#include <memory>
#include <vector>

class AchievementNotificationModel {
	private:
	std::string _id;
	std::string _achievement_user_id;
	std::string _notification_id;

	public:
	AchievementNotificationModel(std::string id, std::string achievement_user_id, std::string notification_id);

	std::string get_id() const;
	std::string get_achievement_user_id() const;
	std::string get_notification_id() const;

	static std::unique_ptr<AchievementNotificationModel> create_notification_service(pqxx::connection& db, const std::string& notification_id, const std::string& achievement_id, bool throw_when_null = false);
};
