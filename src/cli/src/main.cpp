#include <ostream>
#include <cstdlib>

#include <ssmk/cli/application.hpp>
#include <ssmk/version.hpp>

int main(int argc, const char** argv) {
	sm::cli::Application app("ssmk");
	app.version(sm::version.full);
	app.description("Root directory of image tree with ssmk.toml");
	int code = EXIT_SUCCESS;

	if (!(code = app.parse(argc, argv))) {
		std::cout << "file: " << app.context().sourceDirectory << std::endl
			<< "verbosity: " << (int)app.cliContext().verbosity << std::endl;
	}

	return code;
}
