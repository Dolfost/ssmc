#include <iostream>
#include <ssmk/ssmk.hpp>
#include <ssmk/exceptions.hpp>

int main(int argc, char** argv) {
	sm::Ssmk s;
	s.context.config.directory = TESTPATH "/projects/paletted";
	try {
		s();
	} catch (sm::ex::PngError& ex) {
		std::cout << ex.path() << ": " << ex.what() << ": " << ex.description();
		return sm::ex::code::Bad;
	} catch (sm::ex::Error& ex) {
		std::cout << ex.what() << ": " << ex.description();
		return sm::ex::code::Bad;
	} catch (std::exception& ex) {
		std::cout << ex.what();
		return sm::ex::code::Bad;
	}

	return sm::ex::code::Good;
}
