#ifndef _SSMK_SSMK_CONTEXT_HPP_
#define _SSMK_SSMK_CONTEXT_HPP_

#include <ssmk/sprite.hpp>

#include <vector>
#include <ostream>
#include <filesystem>

namespace sm {

struct Context {
	struct Config {
		std::filesystem::path directory;
		std::filesystem::path file;
	} config;

	struct Input {
		std::vector<std::filesystem::path> files;
	} input;

	struct Output {
		std::filesystem::path file;
	} output;

	struct Intermediate {
		int maxBitDepth;
		int maxColorType;
		std::vector<Sprite> sprites;
	} im;

	friend std::ostream& operator<<(std::ostream& os, const Context& c) {
		#define S(PROP) os << #PROP ": " << c.PROP << std::endl;
		#define SV(PROP) \
			os << #PROP ": \n"; \
			for (const auto& r : c.PROP) { \
				os << "  " << r << '\n'; \
			}

		SV(input.files)

		S(config.directory)
		S(output.file)
		S(config.file)

		SV(im.sprites)
		S(im.maxBitDepth)
		S(im.maxColorType)

		return os;

		#undef S
		#undef SV
	}
};

}

#endif // !_SSMK_SSMK_CONTEXT_HPP_
