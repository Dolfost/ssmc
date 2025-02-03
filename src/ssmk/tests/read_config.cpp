#include <iostream>
#include <ssmk/ssmk.hpp>
#include <ssmk/exceptions.hpp>

int main(int argc, char** argv) {
	sm::ssmk s;
	try {
		s.read_config(TESTPATH "/projects/ex1");
	} catch (std::exception& ex) {
		std::cout << ex.what();
		return sm::ex::code::bad;
	}

	return sm::ex::code::good;
}
