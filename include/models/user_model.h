#include <pqxx/pqxx>
#include <string>

class UserModel {
	private:
	std::string _id;
	std::string _password;
	std::string _mail;
	std::string _username;
	std::string _image_url;
	int _balance;
	std::string _type;

	public:
	UserModel(std::string id);

	std::string getId();

	static UserModel create_user(pqxx::connection& db, std::string password, std::string email, std::string username, std::string image_url, int balance, std::string type);
	static bool user_exist(pqxx::connection& db, std::string email);
	static bool username_exist(pqxx::connection& db, std::string username);

};
