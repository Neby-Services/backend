#include <models/achievement_model.h>

AchievementModel::AchievementModel(std::string title, std::string description, int reward) : _title(title), _description(description), _reward(reward) {}

// * GETTERS
std::string AchievementModel::get_description() const { return _description; }
std::string AchievementModel::get_title() const { return _title; }
int AchievementModel::get_reward() const { return _reward; }
