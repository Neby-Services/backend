#include <models/community_model.h>

CommunityModel::CommunityModel(std::string id, std::string name, std::string code, std::string created_at, std::string updated_at) : _id(id), _name(name), _code(code), _created_at(created_at), _updated_at(updated_at) {}

std::string CommunityModel::get_id() const { return _id; }
std::string CommunityModel::get_name() const { return _name; }
std::string CommunityModel::get_code() const { return _code; }
std::string CommunityModel::get_created_at() const { return _created_at; }
std::string CommunityModel::get_updated_at() const { return _updated_at; }

std::string CommunityModel::generate_community_code() {
	const std::string charset = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	const int codeLength = 8;
	std::string code;

	std::srand(std::time(nullptr));

	for (int i = 0; i < codeLength; ++i) {
		code += charset[std::rand() % charset.size()];
	}

	return code;
}

std::unique_ptr<CommunityModel> CommunityModel::create_community(pqxx::connection& db, const std::string& name, bool throw_when_null) {
	pqxx::work txn(db);

	std::string code = generate_community_code();

	pqxx::result result = txn.exec_params("INSERT INTO communities (name, code) VALUES ($1, $2) RETURNING id, name, code, created_at, updated_at", name, code);

	txn.commit();

	if (result.empty()) {
		if (throw_when_null)
			throw creation_exception("community could not be created");
		else
			return nullptr;
	}

	return std::make_unique<CommunityModel>(
		result[0]["id"].as<std::string>(),
		result[0]["name"].as<std::string>(),
		result[0]["code"].as<std::string>(),
		result[0]["created_at"].as<std::string>(),
		result[0]["updated_at"].as<std::string>());
}

std::unique_ptr<CommunityModel> get_community(pqxx::connection& db, const std::string& column, const std::string& value, bool throw_when_null) {
	pqxx::work txn(db);
	pqxx::result result = txn.exec_params(std::format("SELECT id, name, code, created_at, updated_at FROM communities WHERE {} = $1", column), value);
	txn.commit();

	if (result.empty()) {
		if (throw_when_null)
			throw data_not_found_exception("community not found");
		else
			return nullptr;
	}

	return std::make_unique<CommunityModel>(
		result[0]["id"].as<std::string>(),
		result[0]["name"].as<std::string>(),
		result[0]["code"].as<std::string>(),
		result[0]["created_at"].as<std::string>(),
		result[0]["updated_at"].as<std::string>());
}

std::unique_ptr<CommunityModel> CommunityModel::get_community_by_id(pqxx::connection& db, const std::string& id, bool throw_when_null) {
	return get_community(db, "id", id, throw_when_null);
}
std::unique_ptr<CommunityModel> CommunityModel::get_community_by_code(pqxx::connection& db, const std::string& code, bool throw_when_null) {
	return get_community(db, "code", code, throw_when_null);
}
