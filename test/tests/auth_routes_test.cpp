#include <common.h>
#include <cpr/cpr.h>
#include <gtest/gtest.h>
#include <cstdlib>	// Para std::getenv
#include <nlohmann/json.hpp>
#include <pqxx/pqxx>
#include <string>
#include <vector>

// Declaración de la función limpiarTablaUsers
void limpiarTablaUsers() {
	try {
		// Establecer la conexión a la base de datos
		std::string connection_string = std::format("dbname={} user={} password={} host={} port={}", DB_NAME, DB_USER, DB_PASSWORD, DB_HOST, DB_PORT);
		pqxx::connection conn(connection_string);
		// pqxx::connection conn("dbname=mydatabase user=myuser password=mypassword hostaddr=127.0.0.1 port=5432");

		if (conn.is_open()) {
			// Crear un objeto de transacción
			pqxx::work txn(conn);

			// Ejecutar la consulta para limpiar la tabla users
			txn.exec("DELETE FROM users");
			txn.exec("DELETE FROM communities");

			// Confirmar la transacción
			txn.commit();

		} else {
			std::cerr << "Error al conectar a la base de datos." << std::endl;
		}
	} catch (const std::exception& e) {
		std::cerr << "Error de excepción: " << e.what() << std::endl;
	}
}
class RegisterValidations : public ::testing::Test {
	protected:
	void TearDown() override {
		// Llamar a la función limpiarTablaUsers después de que se complete el test
		limpiarTablaUsers();
	}
};

// ** ---------- MISSING FIELDS ON REQ.BODY TESTS ---------- ** \\

TEST(REGISTER_MISSING_FIELDS, MissingEmail) {
	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/auth/register";

	nlohmann::json post_data = {
		{"password", "F!sh1ngR0ck5"},
		{"type", "admin"},
		{"username", "desssddddggdddddsssssdndis"},
		{"community_name", "test_jwsssssst_register"}};

	auto response = cpr::Post(cpr::Url{url}, cpr::Body{post_data.dump()}, cpr::Header{{"Content-Type", "application/json"}});

	EXPECT_EQ(response.status_code, 404) << "expect 404 status code with email missing";
}

TEST(REGISTER_MISSING_FIELDS, MissingUsername) {
	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/auth/register";

	nlohmann::json post_data = {
		{"email", "example@gmail.com"},
		{"password", "F!sh1ngR0ck5"},
		{"type", "admin"},
		{"community_name", "test_jwsssssst_register"}};

	auto response = cpr::Post(cpr::Url{url}, cpr::Body{post_data.dump()}, cpr::Header{{"Content-Type", "application/json"}});

	EXPECT_EQ(response.status_code, 404) << "expect 404 status code with username missing";
}

TEST(REGISTER_MISSING_FIELDS, MissingPassword) {
	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/auth/register";

	nlohmann::json post_data = {
		{"email", "example@gmail.com"},
		{"type", "admin"},
		{"username", "desssddddggdddddsssssdndis"},
		{"community_name", "test_jwsssssst_register"}};

	auto response = cpr::Post(cpr::Url{url}, cpr::Body{post_data.dump()}, cpr::Header{{"Content-Type", "application/json"}});

	EXPECT_EQ(response.status_code, 404) << "expect 404 status code with password missing";
}

TEST(REGISTER_MISSING_FIELDS, MissingType) {
	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/auth/register";

	nlohmann::json post_data = {
		{"email", "example@gmail.com"},
		{"password", "F!sh1ngR0ck5"},
		{"username", "example"},
		{"community_name", "test_jwsssssst_register"}};

	auto response = cpr::Post(cpr::Url{url}, cpr::Body{post_data.dump()}, cpr::Header{{"Content-Type", "application/json"}});

	EXPECT_EQ(response.status_code, 404) << "expect 404 status code with type missing";
}

TEST(REGISTER_MISSING_FIELDS, MissingCommunityName) {
	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/auth/register";

	nlohmann::json post_data = {
		{"email", "example@gmail.com"},
		{"password", "F!sh1ngR0ck5"},
		{"type", "admin"},
		{"username", "example"},
	};

	auto response = cpr::Post(cpr::Url{url}, cpr::Body{post_data.dump()}, cpr::Header{{"Content-Type", "application/json"}});

	EXPECT_EQ(response.status_code, 404) << "expect 404 status code with community_name missing";
}

TEST(REGISTER_MISSING_FIELDS, MissingCommunityCode) {
	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/auth/register";

	nlohmann::json post_data = {
		{"email", "example@gmail.com"},
		{"password", "F!sh1ngR0ck5"},
		{"type", "neighbor"},
		{"username", "example"},
	};

	auto response = cpr::Post(cpr::Url{url}, cpr::Body{post_data.dump()}, cpr::Header{{"Content-Type", "application/json"}});

	EXPECT_EQ(response.status_code, 404) << "expect 404 status code with community_code missing";
}

// ** ---------- VALIDATION REQ.BODY FIELDS TESTS ---------- ** \\

TEST_F(RegisterValidations, CorrectEmail) {
	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/auth/register";

	// Lista de direcciones de correo electrónico correctas
	std::vector<std::string> correct_emails = {
		"user@example.com",
		"user123@example.com",
		"user.name@example.com",
		"user_name@example.com",
		"user+name@example.com",
		"user-name@example.com",
		"user@example.co",
		"user@example.co.uk",
		"user@example.xyz"};

	for (const auto& email : correct_emails) {
		// Verificar que el correo electrónico sea correcto antes de enviar la solicitud
		nlohmann::json post_data = {
			{"email", email},
			{"password", "F!sh1ngR0ck5"},
			{"type", "admin"},
			{"username", "example"},
			{"community_name", "example_community_name"}};

		auto response = cpr::Post(cpr::Url{url}, cpr::Body{post_data.dump()}, cpr::Header{{"Content-Type", "application/json"}});

		// Verificar que el servidor responda con el código de estado 201 (Created)
		EXPECT_EQ(response.status_code, 201) << "Expected 201 status code for correct email: " << email;
		// Limpiar la tabla users después de cada prueba
		limpiarTablaUsers();
	}
}

TEST_F(RegisterValidations, IncorrectEmail) {
	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/auth/register";

	// Lista de direcciones de correo electrónico incorrectas
	std::vector<std::string> incorrect_emails = {
		"example%@gmail.com",
		"example@domain.",
		"example@domain123",
		"example@domain,com",
		"example@domain.com.",
		"example@@domain.com",
		"example@domain..com",
		"example@@domain..com",
		"example@domain_com",
		"example@domain.com_com"};

	for (const auto& email : incorrect_emails) {
		nlohmann::json post_data = {
			{"email", email},
			{"password", "F!sh1ngR0ck5"},
			{"type", "admin"},
			{"username", "example"},
			{"community_name", "example_community_name"}};

		auto response = cpr::Post(cpr::Url{url}, cpr::Body{post_data.dump()}, cpr::Header{{"Content-Type", "application/json"}});

		EXPECT_EQ(response.status_code, 400) << "Expected 400 status code for incorrect email: " << email;
	}
}

TEST_F(RegisterValidations, Correct_password) {
	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/auth/register";

	// Lista de direcciones de correo electrónico incorrectas
	std::vector<std::string> incorrect_passwords = {
		"Tr0ub4dor&3",
		"P@ssw0rd!",
		"S3cur3P@ss",
		"B1gB@ngTh3ory",
		"F!sh1ngR0ck5"};

	for (const auto& password : incorrect_passwords) {
		nlohmann::json post_data = {
			{"email", "example@gmail.com"},
			{"username", "example"},
			{"password", password},
			{"type", "admin"},
			{"community_name", "example_community_name"}};

		auto response = cpr::Post(cpr::Url{url}, cpr::Body{post_data.dump()}, cpr::Header{{"Content-Type", "application/json"}});

		EXPECT_EQ(response.status_code, 201) << "Expected 201 status code for incorrect password: " << password;
		limpiarTablaUsers();
	}
}

TEST_F(RegisterValidations, Incorrect_Password) {
	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/auth/register";

	// Lista de direcciones de correo electrónico incorrectas
	std::vector<std::string> incorrect_passwords = {
		"password", "12345678", "qwerty", "letmein", "abc123"};

	for (const auto& password : incorrect_passwords) {
		nlohmann::json post_data = {
			{"email", "example@gmail.com"},
			{"username", "example"},
			{"password", password},
			{"type", "admin"},
			{"community_name", "example_community_name"}};

		auto response = cpr::Post(cpr::Url{url}, cpr::Body{post_data.dump()}, cpr::Header{{"Content-Type", "application/json"}});

		EXPECT_EQ(response.status_code, 400) << "Expected 400 status code for incorrect password: " << password;
	}
}

TEST_F(RegisterValidations, Incorrect_Username) {
	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/auth/register";

	// Lista de direcciones de correo electrónico incorrectas
	std::vector<std::string> incorrect_usernames = {
		"Invalid!User",
		"SpacesUser ",
		"Short",
		"LongUsernameWithTooManyCharacters",
		"Invalid Spaces"};

	for (const auto& username : incorrect_usernames) {
		nlohmann::json post_data = {
			{"email", "example@gmail.com"},
			{"username", username},
			{"password", "P@ssw0rd!"},
			{"type", "admin"},
			{"community_name", "example_community_name"}};

		auto response = cpr::Post(cpr::Url{url}, cpr::Body{post_data.dump()}, cpr::Header{{"Content-Type", "application/json"}});

		EXPECT_EQ(response.status_code, 400) << "Expected 400 status code for incorrect username: " << username;
	}
}

TEST_F(RegisterValidations, Correct_username) {
	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/auth/register";

	// Lista de direcciones de correo electrónico incorrectas
	std::vector<std::string> incorrect_usernames = {
		"SecureUser1",
		"Usuario123",
		"AlphaBeta45",
		"GoodPassword23",
		"ValidUsername"};

	for (const auto& username : incorrect_usernames) {
		nlohmann::json post_data = {
			{"email", "example@gmail.com"},
			{"username", username},
			{"password", "P@ssw0rd!"},
			{"type", "admin"},
			{"community_name", "example_community_name"}};

		auto response = cpr::Post(cpr::Url{url}, cpr::Body{post_data.dump()}, cpr::Header{{"Content-Type", "application/json"}});

		EXPECT_EQ(response.status_code, 201) << "Expected 201 status code for incorrect username: " << username;
		limpiarTablaUsers();
	}
}

TEST_F(RegisterValidations, Incorrect_type) {
	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/auth/register";

	std::string incorrect_type = "type_error";

	nlohmann::json post_data = {
		{"email", "example@gmail.com"},
		{"username", "example"},
		{"password", "P@ssw0rd!"},
		{"type", incorrect_type},
		{"community_name", "example_community_name"}};

	auto response = cpr::Post(cpr::Url{url}, cpr::Body{post_data.dump()}, cpr::Header{{"Content-Type", "application/json"}});

	EXPECT_EQ(response.status_code, 400) << "Expected 400 status code for incorrect username: " << incorrect_type;
}

// ** ---------- GENERAL ERRORS TESTS ---------- ** \\

class RegisterGeneralErrors : public ::testing::Test {
	protected:
	void TearDown() override {
		limpiarTablaUsers();
	}
};

TEST_F(RegisterGeneralErrors, UserAlredyExist) {
	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/auth/register";

	nlohmann::json new_user = {
		{"email", "example@gmail.com"},
		{"username", "example"},
		{"password", "P@ssw0rd!"},
		{"type", "admin"},
		{"community_name", "example_community_name"}};

	nlohmann::json user_exist_email = {
		{"email", "example@gmail.com"},
		{"username", "example1"},
		{"password", "P@ssw0rd!"},
		{"type", "admin"},
		{"community_name", "example_community_name"}};
	nlohmann::json user_exist_username = {
		{"email", "example@gmail.com"},
		{"username", "example1"},
		{"password", "P@ssw0rd!"},
		{"type", "admin"},
		{"community_name", "example_community_name"}};

	auto response = cpr::Post(cpr::Url{url}, cpr::Body{new_user.dump()}, cpr::Header{{"Content-Type", "application/json"}});

	auto response_email = cpr::Post(cpr::Url{url}, cpr::Body{user_exist_email.dump()}, cpr::Header{{"Content-Type", "application/json"}});

	auto json = nlohmann::json::parse(response_email.text);

	ASSERT_TRUE(json.contains("error"));
	std::string error_message_email = json["error"];
	EXPECT_EQ(error_message_email, "user already exists");
	EXPECT_EQ(response_email.status_code, 400);

	auto response_username = cpr::Post(cpr::Url{url}, cpr::Body{user_exist_username.dump()}, cpr::Header{{"Content-Type", "application/json"}});

	json = nlohmann::json::parse(response_username.text);

	ASSERT_TRUE(json.contains("error"));
	std::string error_message_username = json["error"];
	EXPECT_EQ(error_message_username, "user already exists");
	EXPECT_EQ(response_username.status_code, 400);
}

TEST_F(RegisterGeneralErrors, CorrectSignup) {
	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/auth/register";

	nlohmann::json new_user = {
		{"email", "example@gmail.com"},
		{"username", "example"},
		{"password", "P@ssw0rd!"},
		{"type", "admin"},
		{"community_name", "example_community_name"}};

	auto response = cpr::Post(cpr::Url{url}, cpr::Body{new_user.dump()}, cpr::Header{{"Content-Type", "application/json"}});

	EXPECT_EQ(response.status_code, 201);

	auto json = nlohmann::json::parse(response.text);

	ASSERT_TRUE(json.contains("id"));

	std::string set_cookie_header = response.header["Set-Cookie"];

	// Buscar la cookie "token" dentro del encabezado "Set-Cookie"
	size_t token_pos = set_cookie_header.find("token=");
	bool token_found = token_pos != std::string::npos;

	// Verificar que se encontró la cookie "token"
	EXPECT_TRUE(token_found);
}

TEST_F(RegisterGeneralErrors, Community_Not_Exists) {
	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/auth/register";

	nlohmann::json new_user = {
		{"email", "example@gmail.com"},
		{"username", "example"},
		{"password", "P@ssw0rd!"},
		{"type", "neighbor"},
		{"community_code", "example_community_code"}};

	auto response = cpr::Post(cpr::Url{url}, cpr::Body{new_user.dump()}, cpr::Header{{"Content-Type", "application/json"}});

	EXPECT_EQ(response.status_code, 404);

	auto json = nlohmann::json::parse(response.text);

	ASSERT_TRUE(json.contains("error"));
	std::string error_message_username = json["error"];
	EXPECT_EQ(error_message_username, "not community exists");
}
