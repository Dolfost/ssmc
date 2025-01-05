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

	std::optional<std::string> outputFile = (*outputTable)["file"].value<std::string>();
	if (not outputFile)
		SM_EX_THROW(ConfigFieldError, ConfigNoOutputFile, context.config.file, "output.file")

	context.output.file = context.config.directory / *outputFile;

	// output.packing
	toml::table* packingTable = (*outputTable)["packing"].as_table();
	if (packingTable) {
		std::optional<std::string> algorithm = (*packingTable)["algorithm"].value<std::string>();
		if (algorithm) {
			context.output.packing.algorithm = 
				Context::Output::Packing::Algorithm::None;
			for (const auto& [k, v]: Context::Output::Packing::algorithmText) {
				if (k == *algorithm) {
					context.output.packing.algorithm = v;
					break;
				}
			}
			if (context.output.packing.algorithm == Context::Output::Packing::Algorithm::None)
				SM_EX_THROW(
					ConfigUnexpectedFieldValue, ConfigUnknownPackingAlgorithm,
					context.config.file, "output.packing.algorithm", *algorithm,
					Context::Output::Packing::algorithmText
				)
		}
		std::optional<std::string> order = (*packingTable)["order"].value<std::string>();
		if (order) {
			for (const auto& [k, v]: Context::Output::Packing::orderText) {
				if (k == *order) {
					context.output.packing.order = v;
					break;
				}
			}
		}
		std::optional<std::string> metric = (*packingTable)["metric"].value<std::string>();
		if (metric) {
			context.output.packing.metric = 
				Context::Output::Packing::Metric::None;
			for (const auto& [k, v]: Context::Output::Packing::metricText) {
				if (k == *metric) {
					context.output.packing.metric = v;
					break;
				}
			}
			if (context.output.packing.metric == Context::Output::Packing::Metric::None)
				SM_EX_THROW(
					ConfigUnexpectedFieldValue, ConfigUnknownPackingMetric,
					context.config.file, "output.packing.metric", *metric,
					Context::Output::Packing::metricText
				)
		}
		if (context.output.packing.algorithm == Context::Output::Packing::Algorithm::TreeFit and 
			context.output.packing.order != Context::Output::Packing::Order::Decreasing)
				SM_EX_THROW(
					ConfigExclusiveFieldValues, ConfigIncreasingTreeFitPacking,
					context.config.file, "output.packing.algorithm", "treeFit",
					"output.packing.order", *order
				)
		std::optional<long long> k = (*packingTable)["k"].value<long long>();
		if (k) {
			if (*k <= 0)
				SM_EX_THROW(
					ConfigUnexpectedFieldValue, 
					ConfigUnexpectedFieldValue, 
					context.config.file,
					"output.packing.k",
					std::to_string(*k),
					std::string(">0")
				);
			context.output.packing.k = *k;
		}
	}

	// output.png
	toml::table* pngTable = (*outputTable)["png"].as_table();
	if (pngTable) {
		std::optional<bool> opaque = (*pngTable)["opaque"].value<bool>();
		if (opaque) 
			context.output.png.opaque = *opaque;
		std::optional<std::string> interlacing = (*pngTable)["interlacing"].value<std::string>();
		if (interlacing) {
			for (const auto& [k, v]: Context::Output::Png::interlacingText) {
				if (k == *interlacing) {
					context.output.png.interlacing = v;
					break;
				}
			}
		}
		toml::array* backgroundArray = (*pngTable)["background"].as_array();
		if (backgroundArray) {
			if (backgroundArray->size() != 3)
				SM_EX_THROW(
					ConfigWrongFieldType, 
					ConfigWrongFieldType,
					context.config.file,
					"output.png.background",
					"array[" + std::to_string(backgroundArray->size()) + "]",
					"array<double>[3]"
				)
			std::size_t idx = 0;
			backgroundArray->for_each([&context, &idx](auto&& e) {
				if constexpr (toml::is_number<decltype(e)>) {
					if (e.get() < 0 or e.get() > 1) {
						SM_EX_THROW(
							ConfigUnexpectedFieldValue, 
							ConfigNotRGB, 
							context.config.file, 
							"output.png.background", 
							std::to_string(e.get()), std::string("[0;1]")
						)
					}
					context.output.png.background[idx++] = e.get();
				} else {
					SM_EX_THROW(ConfigWrongFieldType, ConfigWrongFieldType, context.config.file, "output.png.background", "array", "array<doublg>")
				}
			});
		}
		std::optional<int> compression = (*pngTable)["compression"].value<int>();
		if (compression)  {
			if (*compression < 0 or *compression > 9)
				SM_EX_THROW(
					ConfigUnexpectedFieldValue, 
					ConfigUnknownCompressionLevel, 
					context.config.file, 
					"output.png.compression", 
					std::to_string(*compression), std::string("[0;9]")
				)
					context.output.png.compression = *compression;
		}

	}
}

#undef THROW_TOML 
#undef RETHROW_TOML_EX

}
