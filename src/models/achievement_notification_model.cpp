#include <models/achievement_notification_model.h>
AchievementNotificationModel::AchievementNotificationModel(std::string id, std::string achievement_user_id, std::string notification_id) : _id(id), _achievement_user_id(achievement_user_id), _notification_id(notification_id) {}

std::string AchievementNotificationModel::get_id() const { return _id; };
std::string AchievementNotificationModel::get_achievement_user_id() const { return _achievement_user_id; };
std::string AchievementNotificationModel::get_notification_id() const { return _notification_id; };

std::unique_ptr<AchievementNotificationModel> AchievementNotificationModel::create_notification_service(pqxx::connection& db, const std::string& notification_id, const std::string& achievement_id, bool throw_when_null) {
	pqxx::work txn(db);

	pqxx::result result = txn.exec_params("INSERT INTO notifications_achievements (achievement_user_id ,notification_id) VALUES ($1, $2) RETURNING id, achievement_user_id, notification_id", achievement_id, notification_id);

	txn.commit();

	if (result.empty()) {
		if (throw_when_null)
			throw creation_exception("notification_service could not be created");
		return nullptr;
	}

	return std::make_unique<AchievementNotificationModel>(
		result[0]["id"].as<std::string>(),
		result[0]["achievement_user_id"].as<std::string>(),
		result[0]["notification_id"].as<std::string>());
}
