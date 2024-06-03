#pragma once

#include <crow.h>
#include <models/user_achievement_model.h>
#include <models/user_model.h>
#include <utils/common.h>
#include <utils/utils.h>
#include <format>
#include <memory>
#include <pqxx/pqxx>
#include <vector>
#include <string>

class UserAchievementController {
public:
	static void get_user_achievements_self(pqxx::connection& db, crow::request& req, crow::response& res);
	static void claim_user_achievement(pqxx::connection& db, crow::request& req, crow::response& res, const std::string& arch_id);
};
