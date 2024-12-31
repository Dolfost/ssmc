#ifndef _SSMK_CONTEXT_HPP_
#define _SSMK_CONTEXT_HPP_

#include <vector>
#include <string>
#include <ostream>
#include <filesystem>

namespace sm {

struct Context {
	std::vector<std::string> inputFiles;
	std::filesystem::path sourceDirectory;
	std::filesystem::path configFile;
	std::filesystem::path outputFile;

	friend std::ostream& operator<<(std::ostream& os, const Context& c) {
		#define S(PROP) << #PROP ": " << c.PROP << std::endl

		os << "fileRegex: ";
		for (const auto& r : c.inputFiles) {
			os << r;
		}

		return os << std::endl
			S(sourceDirectory)
			S(outputFile)
			S(configFile);

		#undef S
	}
};

}

#endif // !_SSMK_CONTEXT_HPP_
