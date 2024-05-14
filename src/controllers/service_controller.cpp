#include <controllers/service_controller.h>

void ServiceController::create_service(pqxx::connection &db, const crow::request &req, crow::response &res) {
	try {
		crow::json::rvalue body = crow::json::load(req.body);

		const std::vector<std::string> required_fields = {"title", "description", "price", "id", "type"};

		if (!validate_required_body_fields(body, required_fields, res)) return;

		std::string title = body["title"].s();
		std::string description = body["description"].s();
		int price = body["price"].i();
		std::string type = body["type"].s();
		std::string creator_id = body["id"].s();

		std::unique_ptr<UserModel> user = UserModel::get_user_by_id(db, creator_id);

		std::cout << "id creator -> " << creator_id << std::endl;

		if (price < 0) {
			handle_error(res, "invalid price", 400);
			return;
		}

		if ((type == ServiceType::REQUESTED) && (price > user.get()->get_balance())) {
			handle_error(res, "not enough money to pay for service", 400); 
			return;
		}

		std::unique_ptr<ServiceModel> service = ServiceModel::create_service(db, creator_id, title, description, price, type, std::nullopt);

		if (!service) {
			handle_error(res, "internal server error", 500);
			return;
		}

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

		res.code = 201;
		res.write(data.dump());
		res.end();
	} catch (const std::exception &e) {
		std::cerr << "Error creating service: " << e.what() << std::endl;
		handle_error(res, "internal server error", 500);
	}
}

void ServiceController::get_service_by_id(pqxx::connection &db, const crow::request &req, crow::response &res, const std::string &service_id) {
	try {
		if (service_id.empty()) {
			handle_error(res, "id must be provided", 400);
			return;
		}

		if (!isValidUUID(service_id)) {
			handle_error(res, "id is invalid", 400);
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
		service_json["creator_id"] = service.get()->get_creator_id();
		if (service.get()->get_buyer_id().has_value())
			service_json["buyer_id"] = service.get()->get_buyer_id().value();
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

	} catch (const std::exception &e) {
		std::cerr << "Error getting service: " << e.what() << std::endl;
		handle_error(res, "internal server error", 500);
	}
}

void ServiceController::get_services(pqxx::connection &db, const crow::request &req, crow::response &res) {
	try {
		crow::json::rvalue body = crow::json::load(req.body);

		std::unique_ptr<UserModel> user = UserModel::get_user_by_id(db, body["id"].s());
		std::vector<std::unique_ptr<ServiceModel>> all_services;

		auto status = req.url_params.get("status");

		if (!status) {
			all_services = ServiceModel::get_services(db, user.get()->get_community_id());

		} else if (status && (std::string(status) == ServiceStatus::CLOSED || std::string(status) == ServiceStatus::OPEN)) {
			all_services = ServiceModel::get_services(db, user.get()->get_community_id(), status);
		} else {
			handle_error(res, "status not valid value", 400);
			return;
		}

		crow::json::wvalue::list services;
		for (unsigned int i = 0; i < all_services.size(); ++i) {
			crow::json::wvalue service;

			service["id"] = all_services[i].get()->get_id();
			service["creator_id"] = all_services[i].get()->get_creator_id();
			if (all_services[i].get()->get_buyer_id().has_value())
				service["buyer_id"] = all_services[i].get()->get_buyer_id().value();
			service["title"] = all_services[i].get()->get_title();
			service["description"] = all_services[i].get()->get_description();
			service["price"] = all_services[i].get()->get_price();
			service["status"] = all_services[i].get()->get_status();
			service["type"] = all_services[i].get()->get_type();
			if (all_services[i].get()->get_image_url().has_value())
				service["image_url"] = all_services[i].get()->get_image_url().value();
			service["created_at"] = all_services[i].get()->get_created_at();
			service["updated_at"] = all_services[i].get()->get_updated_at();

			if (all_services[i].get()->get_creator().has_value()) {
				crow::json::wvalue creator;
				creator["id"] = all_services[i].get()->get_creator().value().get_id();
				creator["username"] = all_services[i].get()->get_creator().value().get_username();
				creator["type"] = all_services[i].get()->get_creator().value().get_type();
				creator["email"] = all_services[i].get()->get_creator().value().get_email();
				creator["balance"] = all_services[i].get()->get_creator().value().get_balance();
				creator["created_at"] = all_services[i].get()->get_creator().value().get_created_at();
				creator["updated_at"] = all_services[i].get()->get_creator().value().get_updated_at();

				service["creator"] = crow::json::wvalue(creator);
			}

			if (all_services[i].get()->get_buyer().has_value()) {
				crow::json::wvalue buyer;
				buyer["id"] = all_services[i].get()->get_buyer().value().get_id();
				buyer["username"] = all_services[i].get()->get_buyer().value().get_username();
				buyer["type"] = all_services[i].get()->get_buyer().value().get_type();
				buyer["email"] = all_services[i].get()->get_buyer().value().get_email();
				buyer["balance"] = all_services[i].get()->get_buyer().value().get_balance();
				buyer["created_at"] = all_services[i].get()->get_buyer().value().get_created_at();
				buyer["updated_at"] = all_services[i].get()->get_buyer().value().get_updated_at();

				service["buyer"] = crow::json::wvalue(buyer);
			}

			services.push_back(service);
		}

		crow::json::wvalue data{{"services", services}};

		res.write(data.dump());
		res.code = 200;
		res.end();
	} catch (const std::exception &e) {
		std::cerr << "Error getting services: " << e.what() << std::endl;
		handle_error(res, "internal server error", 500);
	}
}

void ServiceController::delete_service(pqxx::connection &db, const crow::request &req, crow::response &res, std::string service_id) {
	try {
		crow::json::rvalue request = crow::json::load(req.body);

		std::unique_ptr<ServiceModel> service = ServiceModel::get_service_by_id(db, service_id, false);

		if (service == nullptr) {
			handle_error(res, "service not found", 404);
			return;
		}

		std::string service_creator_id = service.get()->get_creator_id();

		std::unique_ptr<UserModel> creator = UserModel::get_user_by_id(db, service_creator_id);
		std::string service_community = creator.get()->get_community_id();

		std::unique_ptr<UserModel> admin = UserModel::get_user_by_id(db, request["id"].s());
		std::string admin_community = admin.get()->get_community_id();

		if ((service_community == admin_community && request["isAdmin"].b() == true) || service_creator_id == request["id"].s()) {
			std::unique_ptr<ServiceModel> deleted_service = ServiceModel::delete_service_by_id(db, service_id);
			if (deleted_service) {
				crow::json::wvalue message({{"message", "service deleted succesfully"}});
				res.write(message.dump());
				res.code = 200;
				res.end();
			} else {
				handle_error(res, "could not delete service", 400);
				return;
			}
		} else {
			handle_error(res, "user without admin privileges or not creator of service", 403);
			return;
		}

	} catch (const std::exception &e) {
		std::cerr << "Error deleting service: " << e.what() << std::endl;
		handle_error(res, "Error deleting service", 500);
	}
}

void ServiceController::update_service(pqxx::connection &db, const crow::request &req, crow::response &res, std::string service_id) {
	try {
		crow::json::rvalue request = crow::json::load(req.body);

		std::unique_ptr<ServiceModel> service = ServiceModel::get_service_by_id(db, service_id, false);

		if (service == nullptr) {
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
			std::string temp_tittle = "", temp_description = "";
			int temp_price = -1;

			if (request.has("tittle")) {
				temp_tittle = request["tittle"].s();
			}

			if (request.has("description")) {
				temp_description = request["description"].s();
			}

			if (request.has("price")) {
				temp_price = request["price"].i();
				if (temp_price < 0) {
					handle_error(res, "invalid price", 400);
					return;
				}
				if (request["type"] == ServiceType::REQUESTED && temp_price < user->get_balance()) {
					handle_error(res, "not enough money to pay for service", 400);
					return;
				}
			}

			bool updated_service = ServiceModel::update_service_by_id(db, service_id, temp_tittle, temp_description, temp_price);
			if (updated_service) {
				crow::json::wvalue message({{"message", "service deleted succesfully"}});
				res.write(message.dump());
				res.code = 200;
				res.end();
			} else {
				handle_error(res, "could not delete service", 400);
				return;
			}
		} else {
			handle_error(res, "user without admin privileges or not creator of service", 403);
			return;
		}

	} catch (const std::exception &e) {
		std::cerr << "Error deleting service: " << e.what() << std::endl;
		handle_error(res, "Error deleting service", 500);
	}
}
