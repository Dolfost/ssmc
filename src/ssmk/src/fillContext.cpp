#include <ssmk/ssmk.hpp>
#include <ssmk/exceptions.hpp>

#include <filesystem>
#include <optional>

#include <toml++/toml.hpp>

namespace sm {

#define RETHROW_TOML_EX() \
throw sm::ex::TomlError( \
	context.conf.file, \
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

void ssmk::fill_context(sm::context& context) {
	context.conf.directory = 
		std::filesystem::absolute(context.conf.directory);
	if (not std::filesystem::is_directory(context.conf.directory))
		SM_EX_THROW(FileError, SourceDirectoryNotFound, context.conf.directory)

	for (const auto& filename: configFilenames) {
		if (std::filesystem::is_regular_file(context.conf.directory / filename)) {
			context.conf.file = context.conf.directory / filename;
			break;
		}
	}
	if (context.conf.file.empty())
		SM_EX_THROW(FileError, ConfigNotFound, context.conf.file)

	toml::table table;
	try {
		table = toml::parse_file(context.conf.file.string());
	} catch (const toml::parse_error& ex) {
		RETHROW_TOML_EX()
	}

	toml::table* inputTable = table["input"].as_table();
	if (not inputTable)
		SM_EX_THROW(ConfigFieldError, ConfigNoInputTable, context.conf.file, "input")

	toml::array* filesArray = (*inputTable)["files"].as_array();
	if (not filesArray)
		SM_EX_THROW(ConfigFieldError, ConfigNoInputFileArray, context.conf.file, "input.files")
	filesArray->for_each([&context](auto&& e) {
		if constexpr (toml::is_string<decltype(e)>) {
			std::filesystem::path path = e.as_string()->get();
			if (path.is_relative())
				path = context.conf.directory / path;
			if (not (std::filesystem::is_regular_file(path) or std::filesystem::is_directory(path)))
				SM_EX_THROW(ConfigFieldError, NotAFileOrDirectory, path, "input.files")

			context.in.files.push_back(path);
		} else {
			SM_EX_THROW(ConfigWrongFieldType, ConfigWrongFieldType, context.conf.file, "input.files", "array", "array<string>")
		}
	});

	toml::table* outputTable = table["output"].as_table();
	if (not outputTable)
		SM_EX_THROW(ConfigFieldError, ConfigNoOutputTable, context.conf.file, "output")

	std::optional<std::string> outputFile = (*outputTable)["file"].value<std::string>();
	if (not outputFile)
		SM_EX_THROW(ConfigFieldError, ConfigNoOutputFile, context.conf.file, "output.file")

	context.out.file = context.conf.directory / *outputFile;

	// output.packing
	toml::table* packingTable = (*outputTable)["packing"].as_table();
	if (packingTable) {
		std::optional<std::string> algorithm = (*packingTable)["algorithm"].value<std::string>();
		if (algorithm) {
			context.out.pack.alg = 
				context::output::packing::algorithm::None;
			for (const auto& [k, v]: context::output::packing::algorithmText) {
				if (k == *algorithm) {
					context.out.pack.alg = v;
					break;
				}
			}
			if (context.out.pack.alg == context::output::packing::algorithm::None)
				SM_EX_THROW(
					ConfigUnexpectedFieldValue, ConfigUnknownPackingAlgorithm,
					context.conf.file, "output.packing.algorithm", *algorithm,
					context::output::packing::algorithmText
				)
		}
		std::optional<std::string> order = (*packingTable)["order"].value<std::string>();
		if (order) {
			for (const auto& [k, v]: context::output::packing::orderText) {
				if (k == *order) {
					context.out.pack.order = v;
					break;
				}
			}
		}
		std::optional<std::string> metric = (*packingTable)["metric"].value<std::string>();
		if (metric) {
			context.out.pack.metric = 
				context::output::packing::SortingMetric::None;
			for (const auto& [k, v]: context::output::packing::metricText) {
				if (k == *metric) {
					context.out.pack.metric = v;
					break;
				}
			}
			if (context.out.pack.metric == context::output::packing::SortingMetric::None)
				SM_EX_THROW(
					ConfigUnexpectedFieldValue, ConfigUnknownPackingMetric,
					context.conf.file, "output.packing.metric", *metric,
					context::output::packing::metricText
				)
		}
		if (context.out.pack.alg == context::output::packing::algorithm::TreeFit and 
			context.out.pack.order != context::output::packing::Ordering::Decreasing)
				SM_EX_THROW(
					ConfigExclusiveFieldValues, ConfigIncreasingTreeFitPacking,
					context.conf.file, "output.packing.algorithm", "treeFit",
					"output.packing.order", *order
				)
		std::optional<long long> k = (*packingTable)["k"].value<long long>();
		if (k) {
			if (*k <= 0)
				SM_EX_THROW(
					ConfigUnexpectedFieldValue, 
					ConfigUnexpectedFieldValue, 
					context.conf.file,
					"output.packing.k",
					std::to_string(*k),
					std::string(">0")
				);
			context.out.pack.k = *k;
		}
	}

	// output.png
	toml::table* pngTable = (*outputTable)["png"].as_table();
	if (pngTable) {
		std::optional<bool> opaque = (*pngTable)["opaque"].value<bool>();
		if (opaque) 
			context.out.png.opaque = *opaque;
		std::optional<std::string> interlacing = (*pngTable)["interlacing"].value<std::string>();
		if (interlacing) {
			for (const auto& [k, v]: context::output::png_info::interlacingText) {
				if (k == *interlacing) {
					context.out.png.interlacing = v;
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
					context.conf.file,
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
							context.conf.file, 
							"output.png.background", 
							std::to_string(e.get()), std::string("[0;1]")
						)
					}
					context.out.png.background[idx++] = e.get();
				} else {
					SM_EX_THROW(ConfigWrongFieldType, ConfigWrongFieldType, context.conf.file, "output.png.background", "array", "array<doublg>")
				}
			});
		}
		std::optional<int> compression = (*pngTable)["compression"].value<int>();
		if (compression)  {
			if (*compression < 0 or *compression > 9)
				SM_EX_THROW(
					ConfigUnexpectedFieldValue, 
					ConfigUnknownCompressionLevel, 
					context.conf.file, 
					"output.png.compression", 
					std::to_string(*compression), std::string("[0;9]")
				)
					context.out.png.compression = *compression;
		}

	}
}

#undef THROW_TOML 
#undef RETHROW_TOML_EX

}
