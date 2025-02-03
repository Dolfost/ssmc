#include <iostream>
#include <ssmk/ssmk.hpp>
#include <ssmk/exceptions.hpp>

int main(int argc, char** argv) {
	sm::ssmk s;
	s.context.conf.directory = TESTPATH "/projects/shortRGBarray";
	try {
		s.read_config();
	} catch (sm::ex::config_wrong_field_type& ex) {
		std::cout << ex.what() << ": " << ex.description() << ": "  << ex.path() << std::endl;
		return sm::ex::code::good;
	} catch (sm::ex::config_field_error& ex) {
		std::cout << ex.what() << ": " << ex.description() << ": " << ex.field() << std::endl;
		return sm::ex::code::bad;
	} catch (sm::ex::error& ex) {
		std::cout << ex.what() << ": " << ex.description() << ": " << std::endl;
		return sm::ex::code::bad;
	}

	std::cout << "No exceptions";

	return sm::ex::code::bad;
}
