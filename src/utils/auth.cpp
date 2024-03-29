#include <utils/auth.h>

std::string SECRET_JWT = std::string(std::getenv("SECRET_JWT"));

std::string create_token(const std::string& userId, const std::string& type) {
	auto token = jwt::create()
					 .set_type("JWS")
					 .set_issuer("auth0")
					 .set_payload_claim("id", jwt::claim(std::string(userId)))
					 .set_payload_claim("type", jwt::claim(std::string(type)))
					 .sign(jwt::algorithm::hs256{SECRET_JWT});

	return token;
}

bool validate_token(const std::string& token) {
	try {
		auto verifier = jwt::verify()
							.with_issuer("auth0")
							.allow_algorithm(jwt::algorithm::hs256{SECRET_JWT});
		auto decoded_token = jwt::decode(token);

		verifier.verify(decoded_token);

		return true;
	} catch (const std::exception& e) {
		std::cerr << e.what() << '\n';
		return false;
	}
}

std::string get_token_cookie(crow::request& req) {
	std::string tokenValue;

	const auto& cookiesHeader = req.get_header_value("Cookie");

	std::istringstream iss(cookiesHeader);

	std::string cookie;
	while (std::getline(iss, cookie, ';')) {
		size_t pos = cookie.find("token=");
		if (pos != std::string::npos) {
			tokenValue = cookie.substr(pos + 6);
			return tokenValue;
		}
	}

	return "";
}
