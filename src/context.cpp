#include <ssmk/context.hpp>

#include <cstdlib>

namespace sm {

Context::Context(std::string name, std::string ver) {
	app.name(name)
		->description("Sprite sheet make program");
	app.option_defaults()
		->always_capture_default(true)
		->ignore_case(false)
		->ignore_underscore(false)
		->group("Options");

	#ifdef _WIN32 
	app.allow_windows_style_options();
	#endif

	app.add_option("-s,--source", sourceDirectory)
		->description("Root directory of image tree with ssmk.toml")
		->default_val("./");

	app.add_option("-v,--verbose", verbosity)
		->description("Verbosity level")
		->check(CLI::Range(Verbosity::Silent, Verbosity::Debug))
		->default_val(Verbosity::Regular);

	app.set_version_flag("--version", ver);
	app.set_help_flag("-h,--help", "Print this message and exit");
}

bool Context::parse(int argc, const char** argv) {
	try {
		app.parse(argc, argv);
	} catch (const CLI::CallForHelp &ex) {
		exit(app.exit(ex));
	} catch (const CLI::CallForVersion &ex) {
		exit(app.exit(ex));
	} catch (const CLI::ParseError &ex) {
		return app.exit(ex);
	}
	return EXIT_SUCCESS;
}

}
