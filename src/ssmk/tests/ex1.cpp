#include <iostream>
#include <ssmk/ssmk.hpp>
#include <ssmk/exceptions.hpp>

int main(int argc, char** argv) {
	sm::ssmk s;
	s.context.conf.directory = TESTPATH "/projects/ex1";
	try {
		s();
	} catch (sm::ex::png_error& ex) {
		std::cout << ex.path() << ": " << ex.what() << ": " << ex.description();
		return sm::ex::code::bad;
	} catch (sm::ex::error& ex) {
		std::cout << ex.what() << ": " << ex.description();
		return sm::ex::code::bad;
	} catch (std::exception& ex) {
		std::cout << ex.what();
		return sm::ex::code::bad;
	}

	return sm::ex::code::good;
}
