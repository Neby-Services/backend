#pragma once

#include <utils/errors.h>
#include <ctime>
#include <format>
#include <memory>
#include <pqxx/pqxx>
#include <string>

class AchievementModel {
	private:
	std::string _title;
	std::string _description;
	int _reward;

	public:
	std::string get_title() const;
	std::string get_description() const;
	int get_reward() const;

	AchievementModel(std::string title, std::string description, int reward);
};
