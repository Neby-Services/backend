#include <utils/auth.h>

std::string create_token(const std::string& userId) {
	auto token = jwt::create()
					 .set_type("JWS")
					 .set_issuer("auth0")
					 .set_payload_claim("id", jwt::claim(std::string(userId)))
					 .sign(jwt::algorithm::hs256{"secret"});
	return token;
}

bool validate_token(const std::string& token) {
	try {
		auto verifier = jwt::verify()
							.with_issuer("auth0")
							.allow_algorithm(jwt::algorithm::hs256{"secret"});
		auto decoded_token = jwt::decode(token);

		verifier.verify(decoded_token);

		return true;
	} catch (const std::exception& e) {
		std::cerr << e.what() << '\n';
		return false;
	}
}
