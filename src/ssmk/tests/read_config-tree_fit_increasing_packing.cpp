#include <iostream>
#include <ssmk/ssmk.hpp>
#include <ssmk/exceptions.hpp>

int main(int argc, char** argv) {
	sm::ssmk s;
	try {
		s.read_config(TESTPATH "/projects/tree_fit_increasing_packing");
	} catch (sm::ex::config_exclusive_field_values& ex) {
		return ex.code() == sm::ex::code::config_increasing_tree_fit_packing ? sm::ex::code::good : sm::ex::code::bad;
	}

	std::cout << "General";

	return sm::ex::code::bad;
}
