#include <ssmk/ssmk.hpp>
#include <ssmk/exceptions.hpp>

int main(int argc, char** argv) {
	sm::Ssmk s;
	s.context.config.directory = TESTPATH "/projects/ex1";
	try {
		s.readConfig();
	} catch (std::exception& ex) {
		return sm::ex::code::Bad;
	}

	return sm::ex::code::Good;
}
