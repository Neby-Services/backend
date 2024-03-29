#pragma once

#include <utils/errors.h>
#include <ctime>
#include <format>
#include <memory>
#include <pqxx/pqxx>
#include <string>

class CommunityModel {
	private:
	std::string _id;
	std::string _name;
	std::string _code;
	std::string _created_at;
	std::string _updated_at;

	public:
	CommunityModel(std::string id, std::string name, std::string code, std::string created_at, std::string updated_at);

	std::string get_id() const;
	std::string get_name() const;
	std::string get_code() const;
	std::string get_created_at() const;
	std::string get_updated_at() const;

	static std::string generate_community_code();

	static std::unique_ptr<CommunityModel> create_community(pqxx::connection& db, const std::string& name, bool throw_when_null = false);
	static std::unique_ptr<CommunityModel> get_community_by_id(pqxx::connection& db, const std::string& id, bool throw_when_null = false);
	static std::unique_ptr<CommunityModel> get_community_by_code(pqxx::connection& db, const std::string& code, bool throw_when_null = false);
};
