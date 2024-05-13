#pragma once

#include <crow.h>
#include <models/notification_model.h>
#include <models/service_model.h>
#include <models/user_model.h>
#include <utils/common.h>
#include <utils/utils.h>
#include <format>
#include <memory>
#include <pqxx/pqxx>
#include <string>

class NotificationController {
	public:
	static void create_notification(pqxx::connection& db, const crow::request& req, crow::response& res, const std::string& service_id);
	static void handle_notification(pqxx::connection& db, const crow::request& req, crow::response& res, const std::string& notification_id);
};
