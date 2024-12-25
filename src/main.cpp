#include <ostream>
#include <cstdlib>

#include <ssmk/context.hpp>
#include <ssmk/version.hpp>

int main(int argc, const char** argv) {
	sm::Context context("ssmk", sm::version.full);
	int code = EXIT_SUCCESS;

	if (!(code = context.parse(argc, argv))) {
		std::cout << "file: " << context.sourceDirectory << std::endl
			<< "verbosity: " << (int)context.verbosity << std::endl;
	}

	return code;
}
