#include <models/service_model.h>

ServiceModel::ServiceModel(std::string id, std::string creator_id, std::string title, std::string description, int price, std::string status, std::string buyer_user_id) : _id(id), _creator_id(creator_id), _title(title), _description(description), _price(price), _status(status), _buyer_user_id(buyer_user_id) {}

std::string ServiceModel::get_id() { return _id; }
std::string ServiceModel::get_creator_id() { return _creator_id; }
std::string ServiceModel::get_title() { return _title; }
std::string ServiceModel::get_description() { return _description; }
int ServiceModel::get_price() { return _price; }
std::string ServiceModel::get_status() { return _status; }
std::string ServiceModel::get_buyer_user_id() { return _buyer_user_id; }

// * -------------------------------------------------------------------

std::unique_ptr<ServiceModel> ServiceModel::create_service(pqxx::connection& db, std::string creator_id, std::string title, std::string description, int price, bool isThrow) {
	pqxx::work txn(db);

	// Ejecutar la consulta para insertar el nuevo servicio en la base de datos
	pqxx::result result = txn.exec_params("INSERT INTO services (creator_id, title, description, price) VALUES ($1, $2, $3, $4) RETURNING id",
										  creator_id,
										  title,
										  description,
										  price);

	// Commit de la transacción
	txn.commit();

	// Verificar si la inserción fue exitosa
	if (result.empty()) {
		if (isThrow)
			throw data_not_found_exception("error create service model");

		else
			return nullptr;
	}

	// Obtener el ID del servicio insertado
	std::string service_id = result[0]["id"].as<std::string>();

	// Crear una instancia de ServiceModel y devolverla
	return std::make_unique<ServiceModel>(service_id, creator_id, title, description, price, "OPEN");
}
