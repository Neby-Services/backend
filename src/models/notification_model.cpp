#include <models/notification_model.h>

NotificationModel::NotificationModel(std::string id, std::string sender_id, std::string service_id, std::string status) : _id(id), _sender_id(sender_id), _service_id(service_id), _status(status) {}

std::string NotificationModel::get_id() const { return _id; }
std::string NotificationModel::get_sender_id() const { return _sender_id; }
std::string NotificationModel::get_service_id() const { return _service_id; }
std::string NotificationModel::get_status() const { return _status; }
