#ifndef _SSMK_SSMK_CONTEXT_HPP_
#define _SSMK_SSMK_CONTEXT_HPP_

#include <ssmk/sprite.hpp>

#include <vector>
#include <array>
#include <iosfwd>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <cstdint>
#include <type_traits>

namespace ca::optim {
template<typename T> class Box2D;
}

namespace sm {

struct context {
	struct Config {
		std::filesystem::path directory;
		std::filesystem::path file;
	} conf;

	struct input {
		std::vector<std::filesystem::path> files;
	} in;

	struct output {
		std::filesystem::path file;
		struct packing {
			enum class algorithm {
				None,
				FirstFit,
				NextFit,
				TreeFit,
			} alg = algorithm::TreeFit;
			const static std::unordered_map<std::string, algorithm> algorithmText;
			enum class Ordering {
				None,
				Decreasing,
				Increasing
			} order = Ordering::Decreasing;
			const static std::unordered_map<std::string, Ordering> orderText;
			enum class SortingMetric {
				None,
				Width,
				Height,
				MaxSide,
				MinSide,
				Perimeter,
				Area,
			} metric = SortingMetric::MinSide;
			const static std::unordered_map<std::string, SortingMetric> metricText;
			std::size_t k = 1;
		} pack;
		struct png_info {
			bool opaque = false;
			enum class Interlacing {
				None, 
				Adam7,
			} interlacing = Interlacing::None;
			const static std::unordered_map<std::string, Interlacing> interlacingText;
			std::array<double, 3> background = {0, 0, 0};
			int compression = -1;
		} png;
	} out;

	struct intermediate {
		std::vector<ca::optim::Box2D<std::size_t>*> sprites;

		std::size_t colorPresent   = 0;
		std::size_t alphaPresent   = 0;
		std::size_t palettePresent = 0;
		std::size_t tRNSPresent    = 0;

		std::size_t width, height; ///< Output dimentions
		void* png = nullptr;  ///< Output png data structure
		void* info = nullptr; ///< Output png info structure
		void* background = nullptr; ///< Output png background structure
		void* rows = nullptr; ///< Output buffer
		int color = 0; ///< Output color mode
		int depth = 0; ///< Output color depth
		void* chunk = nullptr; ///< Ssmk specific png chunk
		std::uint32_t chunkSize = 0;
		~intermediate();
	} im;

	friend std::ostream& operator<<(std::ostream& os, const context& c) {
		#define S(PROP) os << #PROP ": " << c.PROP << std::endl;
		#define SE(PROP) os << #PROP ": " << static_cast<std::underlying_type<decltype(c.PROP)>::type>(c.PROP) << std::endl;
		#define SV(PROP) \
		os << #PROP ": \n"; \
		for (const auto& r : c.PROP) { \
			os << "  " << r << '\n'; \
		}

		SV(in.files);

		S(conf.directory);
		S(out.file);

		SE(out.pack.alg);
		SE(out.pack.order);
		SE(out.pack.metric);
		S(out.pack.k);
		SE(out.png.interlacing);
		S(out.png.opaque);
		SV(out.png.background);

		S(conf.file);

		os << "im.sprites" ": \n";
		for (const auto& r : c.im.sprites) {
			os << "  " << *static_cast<Sprite*>(r) << '\n';
		}
		S(im.depth);
		S(im.colorPresent);
		S(im.width);
		S(im.height);

		return os;

		#undef S
		#undef SV
	}

};

}

#endif // !_SSMK_SSMK_CONTEXT_HPP_
