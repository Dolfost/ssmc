#include <iostream>
#include <ssmk/ssmk.hpp>
#include <ssmk/exceptions.hpp>

int main(int argc, char** argv) {
	sm::ssmk s;
	try {
		s.make_sheet(TESTPATH "/projects/generated");
	} catch (sm::ex::error& ex) {
		std::cout << ex.what() << ": " << ex.description();
		return sm::ex::code::bad;
	} catch (std::exception& ex) {
		std::cout << ex.what();
		return sm::ex::code::bad;
	}

	return sm::ex::code::good;
}
