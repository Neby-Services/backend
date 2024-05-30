#include <models/notification_model.h>

NotificationModel::NotificationModel(std::string id, std::string type, std::string created_at, std::string updated_at, std::optional<NotificationServiceModel> notification_service, std::optional<AchievementNotificationModel> achievements_notification) : _id(id), _type(type), _created_at(created_at), _updated_at(updated_at), _notification_service(notification_service), _achievement_notification(achievements_notification) {}

std::string NotificationModel::get_id() const { return _id; }
std::string NotificationModel::get_type() const { return _type; }
std::string NotificationModel::get_created_at() const { return _created_at; }
std::string NotificationModel::get_updated_at() const { return _updated_at; }
std::optional<NotificationServiceModel> NotificationModel::get_notification_service() { return _notification_service; }
std::optional<AchievementNotificationModel> NotificationModel::get_achievement_notification() { return _achievement_notification; };

std::unique_ptr<NotificationModel> NotificationModel::create_notification(pqxx::connection& db, const std::string& type, bool throw_when_null) {
	pqxx::work txn(db);

	pqxx::result result = txn.exec_params("INSERT INTO notifications (type) VALUES ($1) RETURNING id, type, created_at, updated_at", type);

	txn.commit();

	if (result.empty()) {
		if (throw_when_null)
			throw creation_exception("notification could not be created");
		return nullptr;
	}

	return std::make_unique<NotificationModel>(
		result[0]["id"].as<std::string>(),
		result[0]["type"].as<std::string>(),
		result[0]["created_at"].as<std::string>(),
		result[0]["updated_at"].as<std::string>(),
		std::nullopt,
		std::nullopt);
}

std::unique_ptr<NotificationModel> NotificationModel::get_notification_service_by_id(pqxx::connection& db, const std::string& sender_id, const std::string& servcie_id, bool throw_when_null) {
	pqxx::work txn(db);

	std::string query = R"(
		SELECT 
			ns.id AS notification_service_id,
			ns.sender_id,
			ns.service_id,
			ns.status AS notification_service_status,
			ns.notification_id,
			n.type AS notification_type,
			n.created_at AS notification_created_at,
			n.updated_at AS notification_updated_at
		FROM 
    		notifications_services ns
		JOIN 
 		    notifications n ON ns.notification_id = n.id
		WHERE 
    		ns.service_id = $1
    	AND ns.sender_id = $2;
	)";

	pqxx::result result = txn.exec_params(query, servcie_id, sender_id);

	txn.commit();

	if (result.empty()) {
		if (throw_when_null)
			throw data_not_found_exception("service not found");
		else
			return nullptr;
	}

	const auto& row = result[0];

	std::optional<NotificationServiceModel> notification_service = NotificationServiceModel(
		row["notification_service_id"].as<std::string>(),
		row["sender_id"].as<std::string>(),
		row["service_id"].as<std::string>(),
		row["notification_service_status"].as<std::string>(),
		row["notification_id"].as<std::string>());

	return std::make_unique<NotificationModel>(
		row["notification_id"].as<std::string>(),
		row["notification_type"].as<std::string>(),
		row["notification_created_at"].as<std::string>(),
		row["notification_updated_at"].as<std::string>(),
		notification_service,
		std::nullopt);
}
std::vector<NotificationModel> NotificationModel::get_notifications_self(pqxx::connection& db, const std::string& user_id, bool throw_when_null) {
    std::vector<NotificationModel> notifications;
    pqxx::work txn(db);

    std::string query_services = R"(
        WITH user_related_services AS (
            SELECT
                n.id::text AS notification_id,
                n.type::text AS notification_type,
                n.created_at AS notification_created_at,
                n.updated_at AS notification_updated_at,
                ns.id::text AS notification_service_id,
                ns.sender_id::text AS service_sender_id,
                ns.service_id::text AS service_service_id,
                ns.status::text AS service_status,
                NULL::text AS achievement_notification_id,
                NULL::text AS achievement_user_id
            FROM
                notifications n
            LEFT JOIN
                notifications_services ns ON n.id = ns.notification_id
            LEFT JOIN
                services s ON ns.service_id = s.id
            WHERE
                s.creator_id = $1
        )
    )";

    std::string query_achievements = R"(
        WITH user_related_achievements AS (
            SELECT
                n.id::text AS notification_id,
                n.type::text AS notification_type,
                n.created_at AS notification_created_at,
                n.updated_at AS notification_updated_at,
                NULL::text AS notification_service_id,
                NULL::text AS service_sender_id,
                NULL::text AS service_service_id,
                NULL::text AS service_status,
                na.id::text AS achievement_notification_id,
                na.achievement_user_id::text AS achievement_user_id
            FROM
                notifications n
            LEFT JOIN
                notifications_achievements na ON n.id = na.notification_id
            LEFT JOIN
                user_achievements ua ON na.achievement_user_id = ua.id
            WHERE
                ua.user_id = $1
        )
    )";

    std::string final_query = R"(
        WITH
            user_related_services AS (
                SELECT
                    n.id::text AS notification_id,
                    n.type::text AS notification_type,
                    n.created_at AS notification_created_at,
                    n.updated_at AS notification_updated_at,
                    ns.id::text AS notification_service_id,
                    ns.sender_id::text AS service_sender_id,
                    ns.service_id::text AS service_service_id,
                    ns.status::text AS service_status,
                    NULL::text AS achievement_notification_id,
                    NULL::text AS achievement_user_id
                FROM
                    notifications n
                LEFT JOIN
                    notifications_services ns ON n.id = ns.notification_id
                LEFT JOIN
                    services s ON ns.service_id = s.id
                WHERE
                    s.creator_id = $1
            ),
            user_related_achievements AS (
                SELECT
                    n.id::text AS notification_id,
                    n.type::text AS notification_type,
                    n.created_at AS notification_created_at,
                    n.updated_at AS notification_updated_at,
                    NULL::text AS notification_service_id,
                    NULL::text AS service_sender_id,
                    NULL::text AS service_service_id,
                    NULL::text AS service_status,
                    na.id::text AS achievement_notification_id,
                    na.achievement_user_id::text AS achievement_user_id
                FROM
                    notifications n
                LEFT JOIN
                    notifications_achievements na ON n.id = na.notification_id
                LEFT JOIN
                    user_achievements ua ON na.achievement_user_id = ua.id
                WHERE
                    ua.user_id = $1
            )
        SELECT
            *
        FROM
            user_related_services
        UNION ALL
        SELECT
            *
        FROM
            user_related_achievements
        ORDER BY
            notification_updated_at DESC;
    )";

    pqxx::result result = txn.exec_params(final_query, user_id);
    txn.commit();

    if (result.empty()) {
        if (throw_when_null)
            throw data_not_found_exception("service not found");
    }
    for (const auto& row : result) {
        std::optional<NotificationServiceModel> notification_service = std::nullopt;
        std::optional<AchievementNotificationModel> achievement_notification = std::nullopt;

        if (!row["notification_service_id"].is_null()) {
            notification_service = NotificationServiceModel(
                row["notification_service_id"].as<std::string>(),
                row["service_sender_id"].as<std::string>(),
                row["service_service_id"].as<std::string>(),
                row["service_status"].as<std::string>(),
                row["notification_id"].as<std::string>());
        }

        if (!row["achievement_notification_id"].is_null()) {
            achievement_notification = AchievementNotificationModel(
                row["achievement_notification_id"].as<std::string>(),
                row["achievement_user_id"].as<std::string>(),
                row["notification_id"].as<std::string>());
        }

        notifications.push_back(NotificationModel(
            row["notification_id"].as<std::string>(),
            row["notification_type"].as<std::string>(),
            row["notification_created_at"].as<std::string>(),
            row["notification_updated_at"].as<std::string>(),
            notification_service,
            achievement_notification));
    }

    return notifications;
}
