#include <iostream>
#include <ssmk/ssmk.hpp>
#include <ssmk/exceptions.hpp>

int main(int argc, char** argv) {
	sm::ssmk s;
	s.context.config.directory = TESTPATH "/projects/treeFitIncreasingPacking";
	try {
		s.read_config();
	} catch (sm::ex::ConfigExclusiveFieldValues& ex) {
		return ex.code() == sm::ex::code::ConfigIncreasingTreeFitPacking ? sm::ex::code::Good : sm::ex::code::Bad;
	}

	std::cout << "General";

	return sm::ex::code::Bad;
}
