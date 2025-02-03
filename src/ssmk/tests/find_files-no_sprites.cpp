#include <iostream>
#include <ssmk/ssmk.hpp>
#include <ssmk/exceptions.hpp>

int main(int argc, char** argv) {
	sm::ssmk s;
	try {
		s.read_config(TESTPATH "/projects/no_sprites" );
		s.find_files();
	} catch (sm::ex::error& ex) {
		std::cout << ex.what() << ": " << ex.description() << std::endl;
		return ex.code() == sm::ex::code::no_sprites_found ? sm::ex::code::good : sm::ex::code::bad;
	} catch (std::exception& ex) {
		return sm::ex::code::bad;
	}

	std::cout << "sprites:\n";
	for (auto const& x : s.context.im.sprites) {
		std::cout << '\t' << x << '\n';
	}
	

	return sm::ex::code::bad;
}
