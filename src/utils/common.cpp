#include <utils/common.h>

const int HTTP_PORT = std::stoi(std::getenv("HTTP_PORT"));
const std::string DB_NAME = std::string(std::getenv("DB_NAME"));
const std::string DB_USER = std::string(std::getenv("DB_USER"));
const std::string DB_PASSWORD = std::string(std::getenv("DB_PASSWORD"));
const std::string DB_HOST = std::string(std::getenv("DB_HOST"));
const int DB_PORT = std::stoi(std::getenv("DB_PORT"));
const std::string connection_string = std::format("dbname={} user={} password={} host={} port={}", DB_NAME, DB_USER, DB_PASSWORD, DB_HOST, DB_PORT);

const std::string Roles::ADMIN = "admin";
const std::string Roles::NEIGHBOR = "neighbor";

const std::string NotificationServicesStatus::PENDING = "pending";
const std::string NotificationServicesStatus::ACCEPTED = "accepted";
const std::string NotificationServicesStatus::REFUSED = "refused";

const std::string NotificationType::SERVICES = "services";
const std::string NotificationType::ACHIEVEMENTS = "achievements";
const std::string NotificationType::REVIEWS = "reviews";

const std::string ServiceStatus::CLOSED = "closed";
const std::string ServiceStatus::OPEN = "open";

const std::string ServiceType::OFFERED = "offered";
const std::string ServiceType::REQUESTED = "requested";

const std::string AchievementStatus::IN_PROGRESS = "in_progress";
const std::string AchievementStatus::COMPLETED = "completed";
const std::string AchievementStatus::CLAIMED = "claimed";

const std::string AchievementsTitles::ACHIEVEMENT_ONE = "Service Showcase: Unveiling 5 Offerings to the World!";
const std::string AchievementsTitles::ACHIEVEMENT_TWO = "Service Spectrum: Fulfilling 5 Requested Offerings!";
const std::string AchievementsTitles::ACHIEVEMENT_THREE = "Neighborhood Pioneer: Initiate 1 Service Exchange";
const std::string AchievementsTitles::ACHIEVEMENT_FOUR = "Neighbor Nexus: Welcome 5 Service Offers with Open Arms";
const std::string AchievementsTitles::ACHIEVEMENT_FIVE = "Community Cohesion: Engage in 5 Vibrant Service Interactions";

const std::vector<std::string> achievements_titles = {
	AchievementsTitles::ACHIEVEMENT_ONE,
	AchievementsTitles::ACHIEVEMENT_TWO,
	AchievementsTitles::ACHIEVEMENT_THREE,
	AchievementsTitles::ACHIEVEMENT_FOUR,
	AchievementsTitles::ACHIEVEMENT_FIVE
};
