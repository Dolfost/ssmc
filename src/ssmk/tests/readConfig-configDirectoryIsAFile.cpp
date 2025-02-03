#include <iostream>
#include <ssmk/ssmk.hpp>
#include <ssmk/exceptions.hpp>

int main(int argc, char** argv) {
	sm::ssmk s;
	s.context.config.directory = TESTPATH "/" TESTFILE;
	try {
		s.read_config();
	} catch (sm::ex::FileError& ex) {
		std::cout << ex.what() << ": " << ex.description() << ": "  << ex.path() << std::endl;
		return sm::ex::code::Good;
	}

	return sm::ex::code::Bad;
}
