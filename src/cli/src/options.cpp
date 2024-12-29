#include <ssmk/cli/application.hpp>

#include <string>
#include <cstdlib>

namespace sm::cli {

Application::Application(std::string name): a_name(name) {
}

void Application::setOptions() {
	a_app.name(a_name)
		->description("Sprite sheet make program");
	a_app.option_defaults()
		->always_capture_default(true)
		->ignore_case(false)
		->ignore_underscore(false)
		->group("Options");

	#ifdef _WIN32 
	a_app.allow_windows_style_options();
	#endif

	a_app.add_option("-s,--source", a_context.sourceDirectory)
		->description("Root directory of image tree with ssmk.toml")
		->type_name("PATH");

	a_app.add_option("-v,--verbose", a_cliContext.verbosity)
		->type_name("LVL")
		->default_str("DFSTR")
		->description("Verbosity level")
		->check(
			CLI::Range(
				Context::Verbosity::Silent, 
				Context::Verbosity::Debug
			)
		);

	a_app.set_version_flag("--version", a_version);
	a_app.set_help_flag("-h,--help", "Print this message and exit");
}

bool Application::parse(int argc, const char** argv) {
	a_cliContext = sm::cli::Context();
	a_context = sm::Context();

	try {
		a_app.parse(argc, argv);
	} catch (const CLI::CallForHelp &ex) {
		exit(a_app.exit(ex));
	} catch (const CLI::CallForVersion &ex) {
		exit(a_app.exit(ex));
	} catch (const CLI::ParseError &ex) {
		return a_app.exit(ex);
	}
	return EXIT_SUCCESS;
}


}
