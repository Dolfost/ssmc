#include <ssmk/cli/application.hpp>

#include <ssmk/ssmk.hpp>
#include <ssmk/exceptions.hpp>

#include <cstdlib>

namespace sm::cli {

int Application::run(int argc, const char** argv) {
	int parseResult = parse(argc, argv);
	if (parseResult != EXIT_SUCCESS)
		return parseResult;

	a_ssmk.context.config.directory = 
		std::filesystem::absolute(context.sourceDirectory);

	std::cout << "CLI context:\n" << context << std::endl;

	int code = EXIT_SUCCESS;

	a_ssmk.set_images_packed_callback([](const Ssmk& sm) {
		std::cout << "SSMK context (after packing):\n" << sm.context << std::endl;
	});

	try {
		a_ssmk();
	} catch (sm::ex::ConfigFieldError& ex) {
		std::cerr << ex.what() << ": " << ex.description() << ": " << ex.field() << std::endl;
		code = ex.code();
	} catch (sm::ex::Error& ex) {
		std::cerr << ex.what() << ": " << ex.description() << std::endl;
		code = ex.code();
	} catch (std::exception& ex) {
		std::cerr << "Error: " << ex.what() << std::endl;
		code = EXIT_FAILURE;
	}

	std::cout << "SSMK context:\n" << a_ssmk.context << std::endl;

	return code;
}

}
