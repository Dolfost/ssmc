#include <ssmk/ssmk.hpp>
#include <ssmk/exceptions.hpp>

#include <filesystem>
#include <optional>

#include <toml++/toml.hpp>

namespace sm {

#define RETHROW_TOML_EX() \
throw sm::ex::TomlError( \
	context.config.file, \
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
	context.config.directory = 
		std::filesystem::absolute(context.config.directory);
	if (not std::filesystem::is_directory(context.config.directory))
		SM_EX_THROW(FileError, SourceDirectoryNotFound, context.config.directory)

	for (const auto& filename: configFilenames) {
		if (std::filesystem::is_regular_file(context.config.directory / filename)) {
			context.config.file = context.config.directory / filename;
			break;
		}
	}
	if (context.config.file.empty())
		SM_EX_THROW(FileError, ConfigNotFound, context.config.file)

	toml::table table;
	try {
		table = toml::parse_file(context.config.file.string());
	} catch (const toml::parse_error& ex) {
		RETHROW_TOML_EX()
	}

	toml::table* inputTable = table["input"].as_table();
	if (not inputTable)
		SM_EX_THROW(ConfigFieldError, ConfigNoInputTable, context.config.file, "input")


	toml::array* filesArray = (*inputTable)["files"].as_array();
	if (not filesArray)
		SM_EX_THROW(ConfigFieldError, ConfigNoInputFileArray, context.config.file, "input.files")
	filesArray->for_each([&context](auto&& e) {
		if constexpr (toml::is_string<decltype(e)>) {
			std::filesystem::path path = e.as_string()->get();
			if (path.is_relative())
				path = context.config.directory / path;
			if (not (std::filesystem::is_regular_file(path) or std::filesystem::is_directory(path)))
				SM_EX_THROW(ConfigFieldError, NotAFileOrDirectory, path, "input.files")

			context.input.files.push_back(path);
		} else {
			SM_EX_THROW(ConfigWrongFieldType, ConfigWrongFieldType, context.config.file, "input.files", "array", "array<string>")
		}
	});

	toml::table* outputTable = table["output"].as_table();
	if (not outputTable)
		SM_EX_THROW(ConfigFieldError, ConfigNoOutputTable, context.config.file, "output")

	std::optional<std::string_view> outputFile = (*outputTable)["file"].value<std::string_view>();
	if (not outputFile)
		SM_EX_THROW(ConfigFieldError, ConfigNoOutputFile, context.config.file, "output.file")

	context.output.file = context.config.directory / *outputFile;
}

#undef THROW_TOML 
#undef RETHROW_TOML_EX

}
