#include <ssmk/cli/application.hpp>
#include <ssmk/cli/version.hpp>

int main(int argc, const char** argv) {
	sm::cli::Application app("ssmk");
	app.setVersion(sm::cli::version.full);
	app.setDescription("Sprite sheet make program");
	return app.run(argc, argv);
}
