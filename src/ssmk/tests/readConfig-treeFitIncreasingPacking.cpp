#include <iostream>
#include <ssmk/ssmk.hpp>
#include <ssmk/exceptions.hpp>

int main(int argc, char** argv) {
	sm::Ssmk s;
	s.context.config.directory = TESTPATH "/projects/treeFitIncreasingPacking";
	try {
		s.readConfig();
	} catch (sm::ex::ConfigExclusiveFieldValues& ex) {
		return ex.code() == sm::ex::code::ConfigDecreasingFirstFitPacking ? sm::ex::code::Good : sm::ex::code::Bad;
	}

	std::cout << "General";

	return sm::ex::code::Bad;
}
