#include <db/utils.h>


std::string join_query_update(const std::vector<std::string>& elements, const std::string& delimiter) {
	std::ostringstream os;
	auto it = elements.begin();
	if (it != elements.end()) {
		os << *it++;
	}
	while (it != elements.end()) {
		os << delimiter << *it++;
	}
	return os.str();
}
