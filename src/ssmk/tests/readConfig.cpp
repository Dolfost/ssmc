#include <iostream>
#include <ssmk/ssmk.hpp>
#include <ssmk/exceptions.hpp>

int main(int argc, char** argv) {
	sm::ssmk s;
	s.context.conf.directory = TESTPATH "/projects/ex1";
	try {
		s.read_config();
	} catch (std::exception& ex) {
		std::cout << ex.what();
		return sm::ex::code::Bad;
	}

	return sm::ex::code::Good;
}
