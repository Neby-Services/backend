#include <controllers/service_controller.h>

void ServiceController::create_service(pqxx::connection& db, crow::request& req, crow::response& res) {
	try {
		crow::json::rvalue body = crow::json::load(req.body);

		const std::vector<std::string> required_fields = {"title", "description", "price", "id", "type"};

		if (!validate_required_body_fields(body, required_fields, res)) return;

		std::string title = body["title"].s();
		std::string description = body["description"].s();
		int price = body["price"].i();
		std::string type = body["type"].s();
		std::string creator_id = body["id"].s();

		std::unique_ptr<UserModel> user = UserModel::get_user_by_id(db, creator_id, true);

		if (price < 0) {
			handle_error(res, "invalid price", 400);
			return;
		}

		if ((type == ServiceType::REQUESTED) && (price > user.get()->get_balance())) {
			handle_error(res, "not enough money to pay for service", 400);
			return;
		}

		std::unique_ptr<ServiceModel> service = ServiceModel::create_service(db, creator_id, title, description, price, type, std::nullopt, true);

		crow::json::wvalue data(
			{{"id", service.get()->get_id()},
			 {"creator_id", service.get()->get_creator_id()},
			 {"title", service.get()->get_title()},
			 {"description", service.get()->get_description()},
			 {"price", service.get()->get_price()},
			 {"status", service.get()->get_status()},
			 {"type", service.get()->get_type()},
			 {"created_at", service.get()->get_created_at()},
			 {"updated_at", service.get()->get_updated_at()}});

		if (service.get()->get_buyer_id().has_value())
			data["buyer_id"] = service.get()->get_buyer_id().value();
		if (service.get()->get_image_url().has_value())
			data["image_url"] = service.get()->get_image_url().value();

		//* Achievements to handle
		std::vector<std::string> vec = {
			AchievementsTitles::ACHIEVEMENT_ONE, AchievementsTitles::ACHIEVEMENT_TWO, AchievementsTitles::ACHIEVEMENT_THREE};

		set_new_body_prop(req, "tags", vec);
		set_new_body_prop(req, "primary_user_id", creator_id);

		res.code = 201;
		res.write(data.dump());
		res.end();
	} catch (const pqxx::data_exception& e) {
		CROW_LOG_ERROR << "PQXX execption: " << e.what();
		handle_error(res, "invalid id", 400);
	} catch (const data_not_found_exception& e) {
		CROW_LOG_ERROR << "Data not found exception: " << e.what();
		handle_error(res, e.what(), 404);
	} catch (const creation_exception& e) {
		CROW_LOG_ERROR << "Creation exception: " << e.what();
		handle_error(res, e.what(), 404);
	} catch (const std::exception& e) {
		CROW_LOG_ERROR << "Error in create_service controller: " << e.what();
		handle_error(res, "internal server error", 500);
	}
}

void ServiceController::get_service_by_id(pqxx::connection& db, const crow::request& req, crow::response& res, const std::string& service_id) {
	try {
		if (service_id.empty()) {
			handle_error(res, "id must be provided", 400);
			return;
		}

		if (!isValidUUID(service_id)) {
			handle_error(res, "service id is invalid", 400);
			return;
		}
		// Consulta el servicio por su ID
		std::unique_ptr<ServiceModel> service = ServiceModel::get_service_by_id(db, service_id);

		// Verifica si se encontró el servicio
		if (service == nullptr) {
			handle_error(res, "service not found", 404);
			return;
		}

		// Construye el objeto JSON del servicio
		crow::json::wvalue service_json;

		service_json["id"] = service.get()->get_id();
		service_json["title"] = service.get()->get_title();
		service_json["description"] = service.get()->get_description();
		service_json["price"] = service.get()->get_price();
		service_json["status"] = service.get()->get_status();
		service_json["type"] = service.get()->get_type();
		if (service.get()->get_image_url().has_value())
			service_json["image_url"] = service.get()->get_image_url().value();
		service_json["created_at"] = service.get()->get_created_at();
		service_json["updated_at"] = service.get()->get_updated_at();

		if (service.get()->get_creator().has_value()) {
			crow::json::wvalue creator;
			creator["id"] = service.get()->get_creator().value().get_id();
			creator["username"] = service.get()->get_creator().value().get_username();
			creator["type"] = service.get()->get_creator().value().get_type();
			creator["email"] = service.get()->get_creator().value().get_email();
			creator["balance"] = service.get()->get_creator().value().get_balance();
			creator["created_at"] = service.get()->get_creator().value().get_created_at();
			creator["updated_at"] = service.get()->get_creator().value().get_updated_at();

			service_json["creator"] = crow::json::wvalue(creator);
		}

		if (service.get()->get_buyer().has_value()) {
			crow::json::wvalue buyer;
			buyer["id"] = service.get()->get_buyer().value().get_id();
			buyer["username"] = service.get()->get_buyer().value().get_username();
			buyer["type"] = service.get()->get_buyer().value().get_type();
			buyer["email"] = service.get()->get_buyer().value().get_email();
			buyer["balance"] = service.get()->get_buyer().value().get_balance();
			buyer["created_at"] = service.get()->get_buyer().value().get_created_at();
			buyer["updated_at"] = service.get()->get_buyer().value().get_updated_at();

			service_json["buyer"] = crow::json::wvalue(buyer);
		}

		crow::json::wvalue data{{"service", service_json}};

		res.write(data.dump());
		res.code = 200;
		res.end();

	} catch (const pqxx::data_exception& e) {
		CROW_LOG_ERROR << "PQXX execption: " << e.what();
		handle_error(res, "invalid id", 400);
	} catch (const data_not_found_exception& e) {
		CROW_LOG_ERROR << "Data not found exception: " << e.what();
		handle_error(res, e.what(), 404);
	} catch (const std::exception& e) {
		CROW_LOG_ERROR << "Error in get_service_by_id: " << e.what();
		handle_error(res, "internal server error", 500);
	}
}

void ServiceController::get_services(pqxx::connection& db, const crow::request& req, crow::response& res) {
	try {
		crow::json::rvalue body = crow::json::load(req.body);

		std::unique_ptr<UserModel> user = UserModel::get_user_by_id(db, body["id"].s());
		std::vector<ServiceModel> all_services;

		auto status = req.url_params.get("status");

		std::map<std::string, std::string> get_services_filters = {
			{"uc.community_id", user.get()->get_community_id()},
		};

		if (!status) {
			all_services = ServiceModel::get_services(db, get_services_filters);

		} else if (status && (std::string(status) == ServiceStatus::CLOSED || std::string(status) == ServiceStatus::OPEN)) {
			get_services_filters["s.status"] = status;
			all_services = ServiceModel::get_services(db, get_services_filters);
		} else {
			handle_error(res, "status not valid value", 400);
			return;
		}

		crow::json::wvalue::list services;
		for (unsigned int i = 0; i < all_services.size(); ++i) {
			crow::json::wvalue service_json;

			ServiceModel service = all_services[i];

			service_json["id"] = service.get_id();
			service_json["creator_id"] = service.get_creator_id();
			if (service.get_buyer_id().has_value())
				service_json["buyer_id"] = service.get_buyer_id().value();
			service_json["title"] = service.get_title();
			service_json["description"] = service.get_description();
			service_json["price"] = service.get_price();
			service_json["status"] = service.get_status();
			service_json["type"] = service.get_type();
			if (service.get_image_url().has_value())
				service_json["image_url"] = service.get_image_url().value();
			service_json["created_at"] = service.get_created_at();
			service_json["updated_at"] = service.get_updated_at();

			if (service.get_creator().has_value()) {
				crow::json::wvalue creator;
				creator["id"] = all_services[i].get_creator().value().get_id();
				creator["username"] = all_services[i].get_creator().value().get_username();
				creator["type"] = all_services[i].get_creator().value().get_type();
				creator["email"] = all_services[i].get_creator().value().get_email();
				creator["balance"] = all_services[i].get_creator().value().get_balance();
				creator["created_at"] = all_services[i].get_creator().value().get_created_at();
				creator["updated_at"] = all_services[i].get_creator().value().get_updated_at();

				service_json["creator"] = crow::json::wvalue(creator);
			}

			if (service.get_buyer().has_value()) {
				crow::json::wvalue buyer;
				buyer["id"] = all_services[i].get_buyer().value().get_id();
				buyer["username"] = all_services[i].get_buyer().value().get_username();
				buyer["type"] = all_services[i].get_buyer().value().get_type();
				buyer["email"] = all_services[i].get_buyer().value().get_email();
				buyer["balance"] = all_services[i].get_buyer().value().get_balance();
				buyer["created_at"] = all_services[i].get_buyer().value().get_created_at();
				buyer["updated_at"] = all_services[i].get_buyer().value().get_updated_at();

				service_json["buyer"] = crow::json::wvalue(buyer);
			}

			services.push_back(service_json);
		}

		crow::json::wvalue data{{"services", services}};

		res.write(data.dump());
		res.code = 200;
		res.end();
	} catch (const pqxx::data_exception& e) {
		CROW_LOG_ERROR << "PQXX execption: " << e.what();
		handle_error(res, "invalid id", 400);
	} catch (const data_not_found_exception& e) {
		CROW_LOG_ERROR << "Data not found exception: " << e.what();
		handle_error(res, e.what(), 404);
	} catch (const std::exception& e) {
		CROW_LOG_ERROR << "Error in get_services controller: " << e.what();
		handle_error(res, "internal server error", 500);
	}
}

void ServiceController::get_services_self(pqxx::connection& db, const crow::request& req, crow::response& res) {
	try {
		crow::json::rvalue get = crow::json::load(req.body);
		std::string user_id = get["id"].s();

		std::vector<ServiceModel> all_services;

		auto status = req.url_params.get("status");

		std::map<std::string, std::string> get_services_filters = {
			{"s.creator_id", user_id},
		};

		if (!status) {
			all_services = ServiceModel::get_services(db, get_services_filters);

		} else if (status && (std::string(status) == ServiceStatus::CLOSED || std::string(status) == ServiceStatus::OPEN)) {
			get_services_filters["s.status"] = status;
			all_services = ServiceModel::get_services(db, get_services_filters);
		} else {
			handle_error(res, "status not valid value", 400);
			return;
		}

		crow::json::wvalue::list services;
		for (unsigned int i = 0; i < all_services.size(); ++i) {
			crow::json::wvalue service_json;

			ServiceModel service = all_services[i];

			service_json["id"] = service.get_id();
			service_json["title"] = service.get_title();
			service_json["description"] = service.get_description();
			service_json["price"] = service.get_price();
			service_json["status"] = service.get_status();
			service_json["type"] = service.get_type();
			if (service.get_image_url().has_value())
				service_json["image_url"] = service.get_image_url().value();
			service_json["created_at"] = service.get_created_at();
			service_json["updated_at"] = service.get_updated_at();

			if (service.get_creator().has_value()) {
				crow::json::wvalue creator_json;

				UserModel creator = service.get_creator().value();

				creator_json["id"] = creator.get_id();
				creator_json["username"] = creator.get_username();
				creator_json["type"] = creator.get_type();
				creator_json["email"] = creator.get_email();
				creator_json["balance"] = creator.get_balance();
				creator_json["created_at"] = creator.get_created_at();
				creator_json["updated_at"] = creator.get_updated_at();

				service_json["creator"] = crow::json::wvalue(creator_json);
			}

			if (service.get_buyer().has_value()) {
				crow::json::wvalue buyer_json;

				UserModel buyer = service.get_buyer().value();

				buyer_json["id"] = buyer.get_id();
				buyer_json["username"] = buyer.get_username();
				buyer_json["type"] = buyer.get_type();
				buyer_json["email"] = buyer.get_email();
				buyer_json["balance"] = buyer.get_balance();
				buyer_json["created_at"] = buyer.get_created_at();
				buyer_json["updated_at"] = buyer.get_updated_at();

				service_json["buyer"] = crow::json::wvalue(buyer_json);
			}

			services.push_back(service_json);
		}

		crow::json::wvalue data{{"self_services", services}};

		res.write(data.dump());
		res.code = 200;
		res.end();
	} catch (const pqxx::data_exception& e) {
		CROW_LOG_ERROR << "PQXX execption: " << e.what();
		handle_error(res, "invalid id", 400);
	} catch (const data_not_found_exception& e) {
		CROW_LOG_ERROR << "Data not found exception: " << e.what();
		handle_error(res, e.what(), 404);
	} catch (const std::exception& e) {
		CROW_LOG_ERROR << "Error in get_services_self: " << e.what();
		handle_error(res, "internal server error", 500);
	}
}

void ServiceController::delete_service(pqxx::connection& db, const crow::request& req, crow::response& res, std::string service_id) {
	try {
		crow::json::rvalue request = crow::json::load(req.body);

		std::unique_ptr<ServiceModel> service = ServiceModel::get_service_by_id(db, service_id, false);

		if (!service) {
			handle_error(res, "service not found", 404);
			return;
		}

		std::string service_creator_id = service.get()->get_creator_id();

		std::unique_ptr<UserModel> creator = UserModel::get_user_by_id(db, service_creator_id);
		std::string service_community = creator.get()->get_community_id();

		std::unique_ptr<UserModel> admin = UserModel::get_user_by_id(db, request["id"].s());
		std::string admin_community = admin.get()->get_community_id();

		if ((service_community == admin_community && request["isAdmin"].b() == true) || service_creator_id == request["id"].s()) {
			ServiceModel::delete_service_by_id(db, service_id, true);

			crow::json::wvalue message({{"message", "service deleted succesfully"}});
			res.write(message.dump());
			res.code = 200;
			res.end();
		} else {
			handle_error(res, "user without admin privileges or not creator of service", 403);
			return;
		}
	} catch (const pqxx::data_exception& e) {
		CROW_LOG_ERROR << "PQXX execption: " << e.what();
		handle_error(res, "invalid id", 400);
	} catch (const deletion_exception& e) {
		CROW_LOG_ERROR << "Delete exception: " << e.what();
		handle_error(res, e.what(), 404);
	} catch (const std::exception& e) {
		CROW_LOG_ERROR << "Error in delete_service controller: " << e.what();
		handle_error(res, "Error deleting service", 500);
	}
}

void ServiceController::update_service(pqxx::connection& db, const crow::request& req, crow::response& res, std::string service_id) {
	try {
		crow::json::rvalue request = crow::json::load(req.body);

		std::unique_ptr<ServiceModel> service = ServiceModel::get_service_by_id(db, service_id, false);

		if (!service) {
			handle_error(res, "service not found", 404);
			return;
		}

		std::string service_creator_id = service.get()->get_creator_id();

		std::unique_ptr<UserModel> creator = UserModel::get_user_by_id(db, service_creator_id);
		std::string service_community = creator.get()->get_community_id();

		std::unique_ptr<UserModel> admin = UserModel::get_user_by_id(db, request["id"].s());
		std::string admin_community = admin.get()->get_community_id();

		if ((service_community == admin_community && request["isAdmin"].b() == true) || service_creator_id == request["id"].s()) {
			// comprobacions
			std::string creator_id = request["id"].s();
			std::unique_ptr<UserModel> user = UserModel::get_user_by_id(db, creator_id);

			std::map<std::string, std::string> service_update_data;

			if (request.has("title")) {
				service_update_data["title"] = request["title"].s();
			}

			if (request.has("description")) {
				service_update_data["description"] = request["description"].s();
			}

			if (request.has("price")) {
				const int price = request["price"].i();
				if (price < 0) {
					handle_error(res, "invalid price", 400);
					return;
				}
				if (service.get()->get_type() == ServiceType::REQUESTED && price < user->get_balance()) {
					handle_error(res, "not enough money to pay for service", 400);
					return;
				}
				service_update_data["price"] = std::to_string(price);
			}

			ServiceModel::update_service_by_id(db, service_id, service_update_data, true);

			crow::json::wvalue message({{"message", "service updated succesfully"}});

			res.write(message.dump());
			res.code = 200;
			res.end();
		} else {
			handle_error(res, "user without admin privileges or not creator of service", 403);
			return;
		}

	} catch (const pqxx::data_exception& e) {
		CROW_LOG_ERROR << "PQXX execption: " << e.what();
		handle_error(res, "invalid id", 400);
	} catch (const update_exception& e) {
		CROW_LOG_ERROR << "Update exception: " << e.what();
		handle_error(res, e.what(), 404);
	} catch (const std::exception& e) {
		CROW_LOG_ERROR << "Error in update_service controller: " << e.what();
		handle_error(res, "Error updating service", 500);
	}
}
