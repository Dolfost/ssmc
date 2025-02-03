#include <iostream>
#include <ssmk/ssmk.hpp>
#include <ssmk/exceptions.hpp>

int main(int argc, char** argv) {
	sm::ssmk s;
	s.context.conf.directory = TESTPATH "/projects/nosprites" ;
	try {
		s.read_config();
		s.find_files();
	} catch (sm::ex::Error& ex) {
		std::cout << ex.what() << ": " << ex.description() << std::endl;
		return ex.code() == sm::ex::code::NoSpritesFound ? sm::ex::code::Good : sm::ex::code::Bad;
	} catch (std::exception& ex) {
		return sm::ex::code::Bad;
	}

	std::cout << "sprites:\n";
	for (auto const& x : s.context.im.sprites) {
		std::cout << '\t' << x << '\n';
	}
	

	return sm::ex::code::Bad;
}
