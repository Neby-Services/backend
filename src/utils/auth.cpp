#include <utils/auth.h>

std::string create_token(const std::string& userId) {
	auto token = jwt::create()
					 .set_type("JWS")
					 .set_issuer("auth0")
					 .set_payload_claim("id", jwt::claim(std::string(userId)))
					 .sign(jwt::algorithm::hs256{"secret"});
	return token;
}

