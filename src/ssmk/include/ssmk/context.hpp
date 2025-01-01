#ifndef _SSMK_SSMK_CONTEXT_HPP_
#define _SSMK_SSMK_CONTEXT_HPP_

#include <ssmk/sprite.hpp>

#include <vector>
#include <ostream>
#include <filesystem>

namespace sm {

struct Context {
	std::vector<std::filesystem::path> inputFiles;
	std::filesystem::path sourceDirectory;
	std::filesystem::path configFile;
	std::filesystem::path outputFile;

	std::vector<Sprite> sprites;

	friend std::ostream& operator<<(std::ostream& os, const Context& c) {
		#define S(PROP) os << #PROP ": " << c.PROP << std::endl;
		#define SV(PROP) \
			os << #PROP ": \n"; \
			for (const auto& r : c.PROP) { \
				os << "\t" << r << '\n'; \
			}

		SV(inputFiles)

		S(sourceDirectory)
		S(outputFile)
		S(configFile)

		SV(sprites)

		return os;

		#undef S
		#undef SV
	}
};

}

#endif // !_SSMK_SSMK_CONTEXT_HPP_
