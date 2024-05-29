
#include <models/notification_service_model.h>

NotificationServiceModel::NotificationServiceModel(std::string id, std::string sender_id, std::string service_id, std::string status, std::string notification_id) : _id(id), _sender_id(sender_id), _service_id(service_id), _status(status), _notification_id(notification_id) {}

std::string NotificationServiceModel::get_id() const { return _id; }
std::string NotificationServiceModel::get_sender_id() const { return _sender_id; }
std::string NotificationServiceModel::get_service_id() const { return _service_id; }
std::string NotificationServiceModel::get_status() const { return _status; }
std::string NotificationServiceModel::get_notification_id() const { return _notification_id; }

std::unique_ptr<NotificationServiceModel> NotificationServiceModel::create_notification_service(pqxx::connection& db, const std::string& notification_id, const std::string& sender_id, const std::string& service_id, const std::string& status, bool throw_when_null) {
	try {
		pqxx::work txn(db);

		pqxx::result result = txn.exec_params("INSERT INTO notifications_services (sender_id, service_id, status, notification_id) VALUES ($1, $2, $3, $4) RETURNING id, sender_id, service_id, status, notification_id", sender_id, service_id, status, notification_id);

		txn.commit();

		if (result.empty()) {
			if (throw_when_null)
				throw creation_exception("notification_service could not be created");
			return nullptr;
		}

		return std::make_unique<NotificationServiceModel>(
			result[0]["id"].as<std::string>(),
			result[0]["sender_id"].as<std::string>(),
			result[0]["service_id"].as<std::string>(),
			result[0]["status"].as<std::string>(),
			result[0]["notification_id"].as<std::string>());
	} catch (const std::exception& e) {
		std::cerr << "Error to create notification: " << e.what() << std::endl;
		return nullptr;
	}
}

bool NotificationServiceModel::is_requested(pqxx::connection& db, const std::string& sender_id, const std::string& service_id, bool throw_when_null) {
	pqxx::work txn(db);

	pqxx::result result = txn.exec_params("SELECT COUNT(*) FROM notifications_services WHERE sender_id = $1 AND service_id = $2", sender_id, service_id);

	txn.commit();

	if (result.empty() && throw_when_null)
		throw data_not_found_exception("notification not found");
 
	if (!result.empty()) {
		int count = result[0][0].as<int>();

		if (count > 0) return true;
	}

	return false;
}

std::unique_ptr<NotificationServiceModel> NotificationServiceModel::handle_notification_status(pqxx::connection& db, const std::string& status, const std::string& notification_id, bool throw_when_null) {
	try {
		pqxx::work txn(db);

		pqxx::result result = txn.exec_params("UPDATE notifications_services SET status = $1 WHERE id = $2 RETURNING *", status, notification_id);

		txn.commit();

		if (result.empty()) {
			if (throw_when_null)
				throw data_not_found_exception("service not found");
			else
				return nullptr;
		}

		return std::make_unique<NotificationServiceModel>(
			result[0]["id"].as<std::string>(),
			result[0]["sender_id"].as<std::string>(),
			result[0]["service_id"].as<std::string>(),
			result[0]["status"].as<std::string>(),
			result[0]["notification_id"].as<std::string>());

	} catch (const std::exception& e) {
		std::cerr << e.what() << '\n';
		return nullptr;
	}
}

bool NotificationServiceModel::refused_notifications(pqxx::connection& db, const std::string& service_id, const std::string& notification_id, bool throw_when_null) {
	pqxx::work txn(db);

	std::string sql = R"(
            UPDATE notifications_services
            SET status = 'refused'
            WHERE service_id = $1
              AND id != $2
              AND status = 'pending'
        )";

	pqxx::result result = txn.exec_params(sql, service_id, notification_id);

	txn.commit();

	if (result.affected_rows() == 0) {
		if (throw_when_null)
			throw update_exception("nothing has been rejected, maybe no service_notification found to reject");
		else
			return false;
	}

	return true;
}

std::unique_ptr<NotificationServiceModel> get_notification_service(pqxx::connection& db, const std::string& column, const std::string& value, bool throw_when_null) {
	pqxx::work txn(db);
	pqxx::result result = txn.exec_params(std::format("SELECT * FROM notifications_services WHERE {} = $1", column), value);
	txn.commit();

	if (result.empty()) {
		if (throw_when_null)
			throw data_not_found_exception("notification_service not found");
		else
			return nullptr;
	}

	return std::make_unique<NotificationServiceModel>(
		result[0]["id"].as<std::string>(),
		result[0]["sender_id"].as<std::string>(),
		result[0]["service_id"].as<std::string>(),
		result[0]["status"].as<std::string>(),
		result[0]["notification_id"].as<std::string>());
}

std::unique_ptr<NotificationServiceModel> NotificationServiceModel::get_notification_by_id(pqxx::connection& db, const std::string& id, bool throw_when_null) {
	return get_notification_service(db, "id", id, throw_when_null);
}

std::vector<NotificationServiceModel> NotificationServiceModel::get_notifications_accepted_self(pqxx::connection& db, const std::string& sender_id, bool throw_when_null) {
	std::vector<NotificationServiceModel> notifications;
	pqxx::work txn(db);

	std::string sql = R"(
            SELECT *  
			FROM notifications_services
			WHERE sender_id = $1
			AND status = 'accepted'
        )";

	pqxx::result result = txn.exec_params(sql, sender_id);

	txn.commit();

	if (result.empty() && throw_when_null)
		throw data_not_found_exception("notifications_services not found");

	for (const auto& row : result) {
		notifications.push_back(NotificationServiceModel(
			row["id"].as<std::string>(),
			row["sender_id"].as<std::string>(),
			row["service_id"].as<std::string>(),
			row["status"].as<std::string>(),
			row["notification_id"].as<std::string>()));
	}

	return notifications;
}
