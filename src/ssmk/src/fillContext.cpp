#include <ssmk/ssmk.hpp>
#include <ssmk/exceptions.hpp>

#include <filesystem>
#include <optional>

#include <toml++/toml.hpp>

namespace sm {

#define RETHROW_TOML_EX() \
throw sm::ex::TomlError( \
	context.configFile, \
	{ ex.source().begin.line, ex.source().begin.column }, \
	{ ex.source().begin.line, ex.source().begin.column }, \
	std::string(ex.description()) \
);

#define THROW_TOML(E, OBJ) \
throw sm::ex::E( \
	*OBJ->source().path, \
	{ OBJ->source().begin.line, OBJ->source().begin.column }, \
	{ OBJ->source().begin.line, OBJ->source().begin.column } \
);

void Ssmk::fillContext(sm::Context& context) {
	context.sourceDirectory = 
		std::filesystem::absolute(context.sourceDirectory);
	if (not std::filesystem::is_directory(context.sourceDirectory))
		SM_EX_THROW(FileError, SourceDirectoryNotFound, context.sourceDirectory)

	for (const auto& filename: configFilenames) {
		if (std::filesystem::is_regular_file(context.sourceDirectory / filename)) {
			context.configFile = context.sourceDirectory / filename;
			break;
		}
	}
	if (context.configFile.empty())
		SM_EX_THROW(FileError, ConfigNotFound, context.configFile)

	toml::table table;
	try {
		table = toml::parse_file(context.configFile.string());
	} catch (const toml::parse_error& ex) {
		RETHROW_TOML_EX()
	}

	toml::table* inputTable = table["input"].as_table();
	if (not inputTable)
		SM_EX_THROW(ConfigFieldError, ConfigNoInputTable, context.configFile, "input")


	toml::array* filesArray = (*inputTable)["files"].as_array();
	if (not filesArray)
		SM_EX_THROW(ConfigFieldError, ConfigNoInputFileArray, context.configFile, "input.files")
	filesArray->for_each([&context](auto&& e) {
		if constexpr (toml::is_string<decltype(e)>) {
			std::filesystem::path path = e.as_string()->get();
			if (path.is_relative())
				path = context.sourceDirectory / path;
			if (not (std::filesystem::is_regular_file(path) or std::filesystem::is_directory(path)))
				SM_EX_THROW(ConfigFieldError, NotAFileOrDirectory, path, "input.files")

			context.inputFiles.push_back(path);
		} else {
			SM_EX_THROW(ConfigWrongFieldType, ConfigWrongFieldType, context.configFile, "input.files", "array", "array<string>")
		}
	});

	toml::table* outputTable = table["output"].as_table();
	if (not outputTable)
		SM_EX_THROW(ConfigFieldError, ConfigNoOutputTable, context.configFile, "output")

	std::optional<std::string_view> outputFile = (*outputTable)["file"].value<std::string_view>();
	if (not outputFile)
		SM_EX_THROW(ConfigFieldError, ConfigNoOutputFile, context.configFile, "output.file")

	context.outputFile = context.sourceDirectory / *outputFile;
}

#undef THROW_TOML 
#undef RETHROW_TOML_EX

}
