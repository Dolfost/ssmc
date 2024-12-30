#include <iostream>
#include <ssmk/ssmk.hpp>
#include <ssmk/exceptions.hpp>

int main(int argc, char** argv) {
	sm::Ssmk s;
	s.context.sourceDirectory = TESTPATH "/" TESTFILE;
	try {
		s();
	} catch (sm::ex::BadPath& ex) {
		std::cout << ex.what() << " " << ex.path() << std::endl;
		return sm::ex::code::Good;
	}

	return sm::ex::code::Bad;
}
