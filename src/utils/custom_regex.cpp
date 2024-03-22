#include <utils/custom_regex.h>

bool is_correct_email(const std::string& email) {
	static const std::regex pattern(R"([a-zA-Z0-9._+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
	static const std::regex doubleDotPattern(R"(.*\.\..*)");

	// Verificar si hay dos puntos consecutivos en el email
	if (std::regex_match(email, doubleDotPattern)) {
		return false;
	}

	// Verificar la estructura básica del correo electrónico
	return std::regex_match(email, pattern);
}

bool is_correct_type(std::string type) {
	return Roles::ADMIN == type || Roles::NEIGHBOR == type;
}

//** GOOD PASSWORDS -> P@ssw0rd!, Tr0ub4dor&3, S3cur3P@ss, B1gB@ngTh3ory, F!sh1ngR0ck5
//** BAD PASSWORDS ->  password, 12345678, qwerty, letmein, abc123
bool is_correct_password(std::string password) {
	// Configuración de requisitos
	int minLength = 8;
	int minLowercase = 1;
	int minUppercase = 1;
	int minNumbers = 1;
	int minSymbols = 1;

	// Contadores para cada tipo de carácter
	int lowercaseCount = 0;
	int uppercaseCount = 0;
	int numbersCount = 0;
	int symbolsCount = 0;
	std::set<char> uniqueCharacters;

	std::regex symbolRegex("[!@#$%^&*()_+\\-=\\[\\]{};':\",./<>?\\\\|]");

	if (int(password.length()) < minLength) {
		return false;
	}

	for (char c : password) {
		uniqueCharacters.insert(c);

		if (std::islower(c)) {
			lowercaseCount++;
		} else if (std::isupper(c)) {
			uppercaseCount++;
		} else if (std::isdigit(c)) {
			numbersCount++;
		} else if (std::regex_match(std::string(1, c), symbolRegex)) {
			symbolsCount++;
		}
	}

	return (lowercaseCount >= minLowercase &&
			uppercaseCount >= minUppercase &&
			numbersCount >= minNumbers &&
			symbolsCount >= minSymbols &&
			int(uniqueCharacters.size()) >= minLength);
}

//** GOOD USERNAMES -> Usuario123, SecureUser1, AlphaBeta45, GoodPassword23, ValidUsername
//** BAD USERNAMES -> Invalid!User, SpacesUser  , Short, LongUsernameWithTooManyCharacters, Invalid Spaces
bool is_correct_username(std::string username) {
	if (username.length() <= 5 || username.length() > 30)
		return false;

	for (char c : username) {
		if (!std::isalnum(c))
			return false;
	}
	return true;
}
