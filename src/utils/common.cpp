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

const std::string NotificationStatus::PENDING = "pending";
const std::string NotificationStatus::ACCEPTED = "accepted";
const std::string NotificationStatus::REFUSED = "refused";

const std::string ServiceStatus::CLOSED = "closed";
const std::string ServiceStatus::OPEN = "open";

const std::string ServiceType::OFFERED = "offered";
const std::string ServiceType::REQUESTED = "requested";
