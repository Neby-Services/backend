#include <db/connection_pool.h>
#include <models/service_model.h>
#include <models/user_achievement_model.h>
#include <models/notification_service_model.h>
#include <utils/common.h>
#include <memory>
#include <pqxx/pqxx>
#include <map>
#include <string>
#include <vector>

class UserAchievementsHandler {
	public:
	static void handler(const std::vector<std::string>& tags, crow::json::rvalue);
	static void handle_achievement_one(const std::string& user_id, const std::string &user_achievement_id); 
	static void handle_achievement_two(const std::string& user_id, const std::string &user_achievement_id);
	static void handle_achievement_three(const std::string& user_id, const std::string &user_achievement_id);
	static void handle_achievement_four(const std::string& user_id, const std::string &user_achievement_id);
	static void handle_achievement_five(const std::string& user_id, const std::string &user_achievement_id);
};

// * - array de tags
