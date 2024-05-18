#include <models/notification_model.h>

NotificationModel::NotificationModel(std::string id, std::string sender_id, std::string service_id, std::string status, std::string created_at, std::string updated_at) : _id(id), _sender_id(sender_id), _service_id(service_id), _status(status), _created_at(created_at), _updated_at(updated_at) {}

std::string NotificationModel::get_id() const { return _id; }
std::string NotificationModel::get_sender_id() const { return _sender_id; }
std::string NotificationModel::get_service_id() const { return _service_id; }
std::string NotificationModel::get_status() const { return _status; }
std::string NotificationModel::get_created_at() const { return _created_at; }
std::string NotificationModel::get_updated_at() const { return _updated_at; }

std::unique_ptr<NotificationModel> NotificationModel::create_notification(pqxx::connection& db, const std::string& sender_id, const std::string& service_id, const std::string& status, bool throw_when_null) {
	try {
		pqxx::work txn(db);

		pqxx::result result = txn.exec_params("INSERT INTO notifications (sender_id, service_id, status) VALUES ($1, $2, $3) RETURNING id, sender_id, service_id, status, created_at, updated_at", sender_id, service_id, status);

		txn.commit();

		if (result.empty()) {
			if (throw_when_null)
				throw creation_exception("notification could not be created");
			return nullptr;
		}

		return std::make_unique<NotificationModel>(
			result[0]["id"].as<std::string>(),
			result[0]["sender_id"].as<std::string>(),
			result[0]["service_id"].as<std::string>(),
			result[0]["status"].as<std::string>(),
			result[0]["created_at"].as<std::string>(),
			result[0]["updated_at"].as<std::string>());
	} catch (const std::exception& e) {
		std::cerr << "Error to create notification: " << e.what() << std::endl;
		return nullptr;
	}
}

bool NotificationModel::is_requested(pqxx::connection& db, const std::string& sender_id) {
	try {
		pqxx::work txn(db);

		pqxx::result result = txn.exec_params("SELECT COUNT(*) FROM notifications WHERE sender_id = $1", sender_id);

		txn.commit();

		if (!result.empty()) {
			int count = result[0][0].as<int>();

			if (count > 0) return true;
		}

		return false;
	} catch (const std::exception& e) {
		std::cerr << "Error to check notification sender_id exists yet: " << e.what() << std::endl;

		return false;
	}
}

std::unique_ptr<NotificationModel> NotificationModel::handle_notification_status(pqxx::connection& db, const std::string& status, const std::string& notification_id, bool throw_when_null) {
	try {
		pqxx::work txn(db);

		pqxx::result result = txn.exec_params("UPDATE notifications SET status = $1 WHERE id = $2 RETURNING *", status, notification_id);

		txn.commit();

		if (result.empty()) {
			if (throw_when_null)
				throw data_not_found_exception("service not found");
			else
				return nullptr;
		}

		return std::make_unique<NotificationModel>(
			result[0]["id"].as<std::string>(),
			result[0]["sender_id"].as<std::string>(),
			result[0]["service_id"].as<std::string>(),
			result[0]["status"].as<std::string>(),
			result[0]["created_at"].as<std::string>(),
			result[0]["updated_at"].as<std::string>());

	} catch (const std::exception& e) {
		std::cerr << e.what() << '\n';
		return nullptr;
	}
}

bool NotificationModel::refused_notifications(pqxx::connection& db, const std::string& service_id, const std::string& notification_id) {
	try {
		pqxx::work txn(db);

		std::string sql = R"(
            UPDATE notifications
            SET status = 'refused', updated_at = CURRENT_TIMESTAMP
            WHERE service_id = $1
              AND id != $2
              AND status = 'pending'
        )";

		pqxx::result result = txn.exec_params(sql, service_id, notification_id);

		txn.commit();

		return true;

	} catch (const std::exception& e) {
		std::cerr << "Error al actualizar las notificaciones: " << e.what() << '\n';
		return false;
	}
}

std::unique_ptr<NotificationModel> get_notification(pqxx::connection& db, const std::string& column, const std::string& value, bool throw_when_null) {
	pqxx::work txn(db);
	pqxx::result result = txn.exec_params(std::format("SELECT * FROM notifications WHERE {} = $1", column), value);
	txn.commit();

	if (result.empty()) {
		if (throw_when_null)
			throw data_not_found_exception("notification not found");
		else
			return nullptr;
	}

	return std::make_unique<NotificationModel>(
		result[0]["id"].as<std::string>(),
		result[0]["sender_id"].as<std::string>(),
		result[0]["service_id"].as<std::string>(),
		result[0]["status"].as<std::string>(),
		result[0]["created_at"].as<std::string>(),
		result[0]["updated_at"].as<std::string>());
}

std::unique_ptr<NotificationModel> NotificationModel::get_notification_by_id(pqxx::connection& db, const std::string& id, bool throw_when_null) {
	return get_notification(db, "id", id, throw_when_null);
}

std::vector<std::unique_ptr<NotificationModel>> NotificationModel::get_notifications_accepted_self(pqxx::connection& db, const std::string& sender_id) {
	try {
		std::vector<std::unique_ptr<NotificationModel>> notifications;
		pqxx::work txn(db);

		std::string sql = R"(
            SELECT * 
			FROM notifications
			WHERE sender_id = $1
			AND status = 'accepted'
        )";

		pqxx::result result = txn.exec_params(sql, sender_id);

		txn.commit();
		for (const auto& row : result) {
			notifications.push_back(std::make_unique<NotificationModel>(
				row["id"].as<std::string>(),
				row["sender_id"].as<std::string>(),
				row["service_id"].as<std::string>(),
				row["status"].as<std::string>(),
				row["created_at"].as<std::string>(),
				row["updated_at"].as<std::string>()));
		}

		return notifications;

	} catch (const std::exception& e) {
		std::cerr << "Error al actualizar las notificaciones: " << e.what() << '\n';
		return {};
	}
}
