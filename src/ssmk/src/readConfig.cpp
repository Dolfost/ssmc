#include <ssmk/ssmk.hpp>
#include <ssmk/exceptions.hpp>

#include <filesystem>

#define TOML_IMPLEMENTATION
#include <toml++/toml.hpp>

namespace sm {

void Ssmk::readConfig() {
	context.sourceDirectory = 
		std::filesystem::absolute(context.sourceDirectory);
	if (not std::filesystem::is_directory(context.sourceDirectory))
		throw sm::ex::BadSourceDirectoryPath();

	for (const auto& filename: configFilenames) {
		if (std::filesystem::is_regular_file(context.sourceDirectory / filename)) {
			context.configFile = filename;
			break;
		}
	}

	if (context.configFile.empty())
		throw sm::ex::ConfigNotFound();
}

}
