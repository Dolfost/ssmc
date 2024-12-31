#include <iostream>
#include <ssmk/ssmk.hpp>
#include <ssmk/exceptions.hpp>

#include <filesystem>
#include <optional>

#include <toml++/toml.hpp>
#define THROW_TOML_EX(E) \
throw sm::ex::E( \
	context.configFile, \
	std::string(ex.description()), \
	{ ex.source().begin.line, ex.source().begin.column }, \
	{ ex.source().begin.line, ex.source().begin.column } \
);

#define THROW_TOML(E, OBJ) \
throw sm::ex::E( \
	*OBJ->source().path, \
	{ OBJ->source().begin.line, OBJ->source().begin.column }, \
	{ OBJ->source().begin.line, OBJ->source().begin.column } \
);

namespace sm {

void Ssmk::readConfig() {
	context.sourceDirectory = 
		std::filesystem::absolute(context.sourceDirectory);
	if (not std::filesystem::is_directory(context.sourceDirectory))
		throw sm::ex::BadSourceDirectoryPath(context.sourceDirectory);

	for (const auto& filename: configFilenames) {
		if (std::filesystem::is_regular_file(context.sourceDirectory / filename)) {
			context.configFile = context.sourceDirectory / filename;
			break;
		}
	}
	if (context.configFile.empty())
		throw sm::ex::ConfigNotFound(context.sourceDirectory);

	toml::table table;
	try {
		table = toml::parse_file(context.configFile.string());
	} catch (const toml::parse_error& ex) {
		THROW_TOML_EX(ParseError)
	}

	toml::table* inputTable = table["input"].as_table();
	if (not inputTable)
		throw sm::ex::NoInputTable(context.configFile);


	toml::array* filesArray = (*inputTable)["files"].as_array();
	if (not filesArray)
		throw sm::ex::NoFileArray(context.configFile);
	if (filesArray->is_homogeneous<std::string>()) {
		for (const auto& e: *filesArray)
		context.inputFiles.push_back(e.as_string()->get());
	} else THROW_TOML(NotHomogeneousArray, filesArray)

	toml::table* outputTable = table["output"].as_table();
	if (not outputTable)
		throw sm::ex::NoInputTable(context.configFile);

	std::optional<std::string_view> outputFile = (*outputTable)["file"].value<std::string_view>();
	if (not outputFile)
		throw sm::ex::NoOutputFile(context.configFile);

	context.outputFile = context.sourceDirectory / *outputFile;

	std::cout << context << std::endl;
}

}
