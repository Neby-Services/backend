#pragma once

#include <crow.h>
#include <utils/common.h>
#include <utils/utils.h>

#include <format>
#include <memory>
#include <pqxx/pqxx>
#include <string>

class NotificationController {
	public:
	static void create_notification(pqxx::connection& db, const crow::request& req, crow::response& res);
};
