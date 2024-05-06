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
				throw creation_exception("service could not be created");
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
