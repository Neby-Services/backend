#include <cpr/cpr.h>
#include <gtest/gtest.h>

#include <cstdlib>	// Para std::getenv
#include <nlohmann/json.hpp>
#include <string>
#include <vector>


int sum(int a, int b) {
	return a + b;
}
/*
std::string extractTokenFromSetCookie(const std::string& setCookieHeader) {

	size_t pos = setCookieHeader.find(";");

	std::string tokenPart = setCookieHeader.substr(0, pos);

	size_t equalPos = tokenPart.find("=");

	std::string token = tokenPart.substr(equalPos + 1);

	return token;
}

std::string simulateLoginAndGetToken() {
	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/auth/register";

	nlohmann::json post_data = {
		{"email", "denssfffssdddddsssssssddis@gmail.com"},
		{"password", "F!sh1ngR0ck5"},
		{"type", "admin"},
		{"username", "desssddddggdddddsssssdndis"},
		{"community_name", "test_jwsssssst_register"}};

	auto response = cpr::Post(cpr::Url{url}, cpr::Body{post_data.dump()}, cpr::Header{{"Content-Type", "application/json"}});
	for (const auto& cookie : response.cookies) {
		std::cout << cookie.GetDomain() << ":";
		std::cout << cookie.IsIncludingSubdomains() << ":";
		std::cout << cookie.GetPath() << ":";
		std::cout << cookie.IsHttpsOnly() << ":";
		std::cout << cookie.GetExpiresString() << ":";
		std::cout << cookie.GetName() << ":";
		std::cout << cookie.GetValue() << std::endl;
	}
	if (response.status_code == 201) {
		std::string token;
		for (const auto& header : response.header) {
			if (header.first == "Set-Cookie") {
				token = extractTokenFromSetCookie(header.second);
				break;
			}
		}

		std::cout << "Valor de la cookie token: " << token << std::endl;

		return token;
	} else {
		std::cerr << "La solicitud no fue exitosa. Código de estado: " << response.status_code << std::endl;
		return "";
	}
} */

/* TEST(test1, PositiveNumbers) {
	EXPECT_EQ(sum(2, 3), 56) << "custom messag test error";
} */

/* TEST(IntegrationTest, HttpRequest) {
	// Simular inicio de sesión y obtener token JWT
	std::string token = simulateLoginAndGetToken();

	// Construir la URL
	std::string url = "http://backend:" + std::to_string(HTTP_PORT) + "/api/users";

	// Crear un objeto de cookies con el token JWT
	cpr::Cookies cookies;
	cookies["token"] = token;

	// Realizar la solicitud GET con las cookies
	auto response = cpr::Get(cpr::Url{url}, cpr::Cookies{cookies});

	EXPECT_EQ(response.status_code, 200);

	auto json = nlohmann::json::parse(response.text);

	ASSERT_TRUE(json.contains("users"));
	ASSERT_TRUE(json["users"].is_array());

	for (const auto& user : json["users"]) {
		ASSERT_TRUE(user.contains("id"));
		ASSERT_TRUE(user.contains("image_url"));
		ASSERT_TRUE(user.contains("email"));
		ASSERT_TRUE(user.contains("username"));
		ASSERT_TRUE(user.contains("balance"));
		ASSERT_TRUE(user.contains("type"));
	}
}
 */