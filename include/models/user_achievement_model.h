#pragma once

#include <models/achievement_model.h>
#include <utils/common.h>
#include <utils/errors.h>
#include <memory>
#include <optional>
#include <vector>

class UserAchievementModel {
	private:
	std::string _id;
	std::string _achievement_title;
	std::string _user_id;
	std::string _status;
	std::optional<AchievementModel> _achievement;

	public:
	std::string get_id() const;
	std::string get_achievement_title() const;
	std::string get_user_id() const;
	std::string get_status() const;
	std::optional<AchievementModel> get_achievement() const;

	UserAchievementModel(std::string id, std::string achievement_title, std::string user_id, std::string status, std::optional<AchievementModel> achievement);

	static std::vector<std::unique_ptr<UserAchievementModel>> create_user_achievement_tables(pqxx::connection& db, const std::vector<std::string>& achievements_titles, const std::string& user_id, bool throw_when_null = false);

	static std::vector<std::unique_ptr<UserAchievementModel>> get_user_achievements_by_id(pqxx::connection& db, const std::string& user_id, const std::string& status = "", bool throw_when_null = false);

	static std::unique_ptr<UserAchievementModel> update_status_by_id(pqxx::connection& db, const std::string& user_achievement_id, const std::string& status, bool throw_when_null = false);
};
