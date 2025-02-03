#include <iostream>
#include <ssmk/ssmk.hpp>
#include <ssmk/exceptions.hpp>

int main(int argc, char** argv) {
	sm::ssmk s;
	s.context.conf.directory = TESTPATH "/projects/notRGB";
	try {
		s.read_config();
	} catch (sm::ex::ConfigUnexpectedFieldValue& ex) {
		std::cout << ex.what() << ": " << ex.description() << ": "  << ex.path() << std::endl;
		return ex.code() == sm::ex::code::ConfigNotRGB ? sm::ex::code::Good : sm::ex::code::Bad;
	} catch (sm::ex::Error& ex) {
		std::cout << ex.what() << ": " << ex.description() << ": " << std::endl;
		return sm::ex::code::Bad;
	}

	std::cout << "No exceptions";

	return sm::ex::code::Bad;
}
