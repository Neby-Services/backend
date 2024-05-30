#include <utils/user_achievements_handler.h>

void UserAchievementsHandler::handler(const std::vector<std::string>& tags, crow::json::rvalue body) {
	ConnectionPool pool(connection_string, 1);
	auto conn = pool.get_connection();
	std::vector<std::unique_ptr<UserAchievementModel>> achievements;
	std::string primary_user_id = "";
	std::string secondary_user_id = "";
	if (body.has("primary_user_id")) {
		primary_user_id = body["primary_user_id"].s();
		achievements = UserAchievementModel::get_user_achievements_by_id(*conn.get(), primary_user_id, AchievementStatus::IN_PROGRESS);
	}
	if (body.has("secondary_user_id")) {
		secondary_user_id = body["secondary_user_id"].s();
		achievements = UserAchievementModel::get_user_achievements_by_id(*conn.get(), secondary_user_id, AchievementStatus::IN_PROGRESS);
	}

	pool.release_connection(conn);

	for (const auto& tag : tags) {
		for (const auto& achievement : achievements) {
			std::string achievement_title = achievement.get()->get_achievement().value().get_title();
			std::string user_achievement_id = achievement.get()->get_id();
			if (tag == achievement_title) {
				if (tag == AchievementsTitles::ACHIEVEMENT_ONE) {
					handle_achievement_one(primary_user_id, user_achievement_id);
				} else if (tag == AchievementsTitles::ACHIEVEMENT_TWO) {
					handle_achievement_two(primary_user_id, user_achievement_id);
				} else if (tag == AchievementsTitles::ACHIEVEMENT_THREE) {
					handle_achievement_three(primary_user_id, user_achievement_id);
				} else if (tag == AchievementsTitles::ACHIEVEMENT_FOUR) {
					handle_achievement_four(primary_user_id, user_achievement_id);
				} else if (tag == AchievementsTitles::ACHIEVEMENT_FIVE) {
					handle_achievement_five(secondary_user_id, user_achievement_id);
				}
				break;
			}
		}
	}
}

void UserAchievementsHandler::handle_achievement_one(const std::string& user_id, const std::string& user_achievement_id) {
	ConnectionPool pool(connection_string, 1);
	auto conn = pool.get_connection();

	std::map<std::string, std::string> get_services_filters = {
		{"s.creator_id", user_id},
		{"s.type", ServiceType::OFFERED}};

	std::vector<ServiceModel> services = ServiceModel::get_services(*conn.get(), get_services_filters);

	if (services.size() >= 5) {
		CROW_LOG_INFO << "achievement completed!";
		std::map<std::string, std::string> fields = {
			{"status", AchievementStatus::COMPLETED}};

		UserAchievementModel::update_by_id(*conn.get(), user_achievement_id, fields, true);

		std::unique_ptr<NotificationModel> notification = NotificationModel::create_notification(*conn.get(), NotificationType::ACHIEVEMENTS, true);

		AchievementNotificationModel::create_notification_service(*conn.get(), notification.get()->get_id(), user_achievement_id, true); 
	}
	pool.release_connection(conn);
}

void UserAchievementsHandler::handle_achievement_two(const std::string& user_id, const std::string& user_achievement_id) {
	ConnectionPool pool(connection_string, 1);
	auto conn = pool.get_connection();

	std::map<std::string, std::string> get_services_filters = {
		{"s.creator_id", user_id},
		{"s.type", ServiceType::REQUESTED}};

	std::vector<ServiceModel> services = ServiceModel::get_services(*conn.get(), get_services_filters);

	if (services.size() >= 5) {
		CROW_LOG_INFO << "achievement completed!";
		std::map<std::string, std::string> fields = {
			{"status", AchievementStatus::COMPLETED}};

		UserAchievementModel::update_by_id(*conn.get(), user_achievement_id, fields, true);

		std::unique_ptr<NotificationModel> notification = NotificationModel::create_notification(*conn.get(), NotificationType::ACHIEVEMENTS, true);

		AchievementNotificationModel::create_notification_service(*conn.get(), notification.get()->get_id(), user_achievement_id, true);
	}
	pool.release_connection(conn);
}

void UserAchievementsHandler::handle_achievement_three(const std::string& user_id, const std::string& user_achievement_id) {
	ConnectionPool pool(connection_string, 1);
	auto conn = pool.get_connection();

	std::map<std::string, std::string> get_services_filters = {
		{"s.creator_id", user_id}};

	std::vector<ServiceModel> services = ServiceModel::get_services(*conn.get(), get_services_filters);
	if (services.size() >= 1) {
		CROW_LOG_INFO << "achievement completed!";
		std::map<std::string, std::string> fields = {
			{"status", AchievementStatus::COMPLETED}};

		UserAchievementModel::update_by_id(*conn.get(), user_achievement_id, fields, true);

		std::unique_ptr<NotificationModel> notification = NotificationModel::create_notification(*conn.get(), NotificationType::ACHIEVEMENTS, true);

		AchievementNotificationModel::create_notification_service(*conn.get(), notification.get()->get_id(), user_achievement_id, true);
	}
	pool.release_connection(conn);
}

void UserAchievementsHandler::handle_achievement_four(const std::string& user_id, const std::string& user_achievement_id) {
	ConnectionPool pool(connection_string, 1);
	auto conn = pool.get_connection();

	std::vector<ServiceModel> services = ServiceModel::get_services_sold_by_creator_id(*conn.get(), user_id);
	if (services.size() >= 5) {
		CROW_LOG_INFO << "achievement completed!";
		std::map<std::string, std::string> fields = {
			{"status", AchievementStatus::COMPLETED}};

		UserAchievementModel::update_by_id(*conn.get(), user_achievement_id, fields, true);

		std::unique_ptr<NotificationModel> notification = NotificationModel::create_notification(*conn.get(), NotificationType::ACHIEVEMENTS, true);

		AchievementNotificationModel::create_notification_service(*conn.get(), notification.get()->get_id(), user_achievement_id, true);
	}
	pool.release_connection(conn);
}

void UserAchievementsHandler::handle_achievement_five(const std::string& user_id, const std::string& user_achievement_id) {
	ConnectionPool pool(connection_string, 1);
	auto conn = pool.get_connection();

	std::vector<NotificationServiceModel> notifications = NotificationServiceModel::get_notifications_accepted_self(*conn.get(), user_id);
	if (notifications.size() >= 5) {
		CROW_LOG_INFO << "achievement completed!";
		std::map<std::string, std::string> fields = {
			{"status", AchievementStatus::COMPLETED}};

		UserAchievementModel::update_by_id(*conn.get(), user_achievement_id, fields, true);


		std::unique_ptr<NotificationModel> notification = NotificationModel::create_notification(*conn.get(), NotificationType::ACHIEVEMENTS, true);

		AchievementNotificationModel::create_notification_service(*conn.get(), notification.get()->get_id(), user_achievement_id, true);
	}
	pool.release_connection(conn);
}
