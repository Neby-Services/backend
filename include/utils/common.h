#pragma once
#include <crow.h>
#include <db/connection_pool.h>
#include <bcrypt/BCrypt.hpp>
#include <cstdlib>
#include <map>
#include <string>
#include <vector>


extern const std::string DB_NAME;
extern const std::string DB_USER;
extern const std::string DB_PASSWORD;
extern const std::string DB_HOST;
extern const int DB_PORT;
extern const int HTTP_PORT;
extern const std::string connection_string;
extern const std::vector<std::string> achievements_titles;

struct Roles {
	static const std::string ADMIN;
	static const std::string NEIGHBOR;
};

struct NotificationServicesStatus {
	static const std::string PENDING;
	static const std::string ACCEPTED;
	static const std::string REFUSED;
};

struct NotificationType {
	static const std::string SERVICES;
	static const std::string ACHIEVEMENTS;
	static const std::string REVIEWS;
};

struct ServiceStatus {
	static const std::string OPEN;
	static const std::string CLOSED;
};

struct ServiceType {
	static const std::string OFFERED;
	static const std::string REQUESTED;
};

struct AchievementStatus {
	static const std::string IN_PROGRESS;
	static const std::string COMPLETED;
	static const std::string CLAIMED;
};

struct AchievementsTitles {
	static const std::string ACHIEVEMENT_ONE;
	static const std::string ACHIEVEMENT_TWO;
	static const std::string ACHIEVEMENT_THREE;
	static const std::string ACHIEVEMENT_FOUR;
	static const std::string ACHIEVEMENT_FIVE;
};
