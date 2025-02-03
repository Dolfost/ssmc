#include <iostream>
#include <ssmk/ssmk.hpp>
#include <ssmk/exceptions.hpp>

int main(int argc, char** argv) {
	sm::ssmk s;
	s.context.conf.directory = TESTPATH;
	try {
		s.read_config();
	} catch (sm::ex::file_error& ex) {
		std::cout << ex.what() << ": " << ex.description() << ": "  << ex.path() << std::endl;
		return sm::ex::code::good;
	}

	return sm::ex::code::bad;
}
