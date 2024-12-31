#include <ssmk/cli/application.hpp>
#include <ssmk/ssmk.hpp>

#include <cstdlib>

namespace sm::cli {

int Application::run(int argc, const char** argv) {
	int parseResult = parse(argc, argv);
	if (parseResult != EXIT_SUCCESS)
		return parseResult;

	a_ssmk.context.sourceDirectory = 
		std::filesystem::absolute(context.sourceDirectory);

	std::cout << "CLI context:\n" << context << std::endl;

	try {
		a_ssmk();
	} catch (std::exception& ex) {
		std::cout << "Error: " << ex.what() << std::endl;
	}

	std::cout << "SSMK context:\n" << a_ssmk.context << std::endl;

	return EXIT_SUCCESS;
}

}
