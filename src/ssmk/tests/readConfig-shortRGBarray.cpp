#include <iostream>
#include <ssmk/ssmk.hpp>
#include <ssmk/exceptions.hpp>

int main(int argc, char** argv) {
	sm::ssmk s;
	s.context.conf.directory = TESTPATH "/projects/shortRGBarray";
	try {
		s.read_config();
	} catch (sm::ex::ConfigWrongFieldType& ex) {
		std::cout << ex.what() << ": " << ex.description() << ": "  << ex.path() << std::endl;
		return sm::ex::code::Good;
	} catch (sm::ex::ConfigFieldError& ex) {
		std::cout << ex.what() << ": " << ex.description() << ": " << ex.field() << std::endl;
		return sm::ex::code::Bad;
	} catch (sm::ex::Error& ex) {
		std::cout << ex.what() << ": " << ex.description() << ": " << std::endl;
		return sm::ex::code::Bad;
	}

	std::cout << "No exceptions";

	return sm::ex::code::Bad;
}
