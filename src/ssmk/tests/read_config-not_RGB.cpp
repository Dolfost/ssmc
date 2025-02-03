#include <iostream>
#include <ssmk/ssmk.hpp>
#include <ssmk/exceptions.hpp>

int main(int argc, char** argv) {
	sm::ssmk s;
	try {
		s.read_config(TESTPATH "/projects/not_RGB");
	} catch (sm::ex::config_unexpected_field_value& ex) {
		std::cout << ex.what() << ": " << ex.description() << ": "  << ex.path() << std::endl;
		return ex.code() == sm::ex::code::config_not_RGB ? sm::ex::code::good : sm::ex::code::bad;
	} catch (sm::ex::error& ex) {
		std::cout << ex.what() << ": " << ex.description() << ": " << std::endl;
		return sm::ex::code::bad;
	}

	std::cout << "No exceptions";

	return sm::ex::code::bad;
}
