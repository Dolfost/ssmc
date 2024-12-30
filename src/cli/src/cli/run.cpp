#include <ssmk/cli/application.hpp>
#include <ssmk/ssmk.hpp>

#include <cstdlib>

namespace sm::cli {

int Application::run(int argc, const char** argv) {
	int parseResult = parse(argc, argv);
	if (parseResult != EXIT_SUCCESS)
		return parseResult;

	a_ssmk();

	return EXIT_SUCCESS;
}

}
