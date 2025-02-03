#include <ssmk/cli/application.hpp>

#include <string>
#include <cstdlib>

#include <ssmk/version.hpp>

namespace sm::cli {

void Application::setOptions() {
	a_app.option_defaults()
		->always_capture_default(true)
		->ignore_case(false)
		->ignore_underscore(false)
		->group("Options");

	#ifdef _WIN32 
	a_app.allow_windows_style_options();
	#endif

	a_app.add_option("-s,--source", context.sourceDirectory)
		->description("Root directory of image tree with ssmk.toml")
		->check(CLI::ExistingDirectory)
		->type_name("PATH");

	a_app.add_option("-v,--verbose", a_context.verbosity)
		->type_name("LVL")
		->description("Verbosity level")
		->check(
			CLI::Range(
				Context::Verbosity::Silent, 
				Context::Verbosity::Debug
			)
		);

	a_app.set_help_flag("-h,--help", "Print this message and exit");
	a_app.add_flag_callback(
		"--ssmk-version",
		[]() { throw CLI::CallForVersion(sm::version.full, EXIT_SUCCESS); },
		"Display ssmk library version information and exit"
	);
}

void Application::setOptionsStrings() {
	a_app.name(a_name)
		->description(a_description);
	a_app.set_version_flag("--version", a_version);
}

int Application::parse(int argc, const char** argv) {
	setOptionsStrings();

	a_context = sm::cli::Context();
	a_ssmk.context = sm::context();

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
