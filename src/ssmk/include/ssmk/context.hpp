#ifndef _SSMK_SSMK_CONTEXT_HPP_
#define _SSMK_SSMK_CONTEXT_HPP_

#include <ssmk/sprite.hpp>

#include <vector>
#include <array>
#include <ostream>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <type_traits>

namespace ca::optim {
template<typename T> class Box2D;
}

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
		struct Packing {
			enum class Algorithm {
				None,
				FirstFit,
				NextFit,
				TreeFit,
			} algorithm = Algorithm::TreeFit;
			const static std::unordered_map<std::string, Algorithm> algorithmText;
			enum class Order {
				None,
				Decreasing,
				Increasing
			} order = Order::Decreasing;
			const static std::unordered_map<std::string, Order> orderText;
			enum class Metric {
				None,
				Width,
				Height,
				MaxSide,
				MinSide,
				Perimeter,
				Area,
			} metric = Metric::MaxSide;
			const static std::unordered_map<std::string, Metric> metricText;
			std::size_t k = 1;
		} packing;
		struct Png {
			bool opaque = false;
			enum class Interlacing {
				None, 
				Adam7,
			} interlacing = Interlacing::None;
			const static std::unordered_map<std::string, Interlacing> interlacingText;
			std::array<double, 3> background = {0, 0, 0};
		} png;
	} output;

	struct Intermediate {
		int maxBitDepth = 0;
		bool maxColor = 0;
		bool maxAlpha = 0;
		std::vector<ca::optim::Box2D<std::size_t>*> sprites;
		~Intermediate() {
			for (auto ptr: sprites) {
				delete ptr;
			}
		}
		std::size_t width, height;
	} im;

	friend std::ostream& operator<<(std::ostream& os, const Context& c) {
		#define S(PROP) os << #PROP ": " << c.PROP << std::endl;
		#define SE(PROP) os << #PROP ": " << static_cast<std::underlying_type<decltype(c.PROP)>::type>(c.PROP) << std::endl;
		#define SV(PROP) \
		os << #PROP ": \n"; \
		for (const auto& r : c.PROP) { \
			os << "  " << r << '\n'; \
		}

		SV(input.files);

		S(config.directory);
		S(output.file);

		SE(output.packing.algorithm);
		SE(output.packing.order);
		SE(output.packing.metric);
		S(output.packing.k);
		SE(output.png.interlacing);
		S(output.png.opaque);
		SV(output.png.background);

		S(config.file);

		os << "im.sprites" ": \n";
		for (const auto& r : c.im.sprites) {
			os << "  " << *static_cast<Sprite*>(r) << '\n';
		}
		S(im.maxBitDepth);
		S(im.maxColor);
		S(im.width);
		S(im.height);

		return os;

		#undef S
		#undef SV
	}

};

}

#endif // !_SSMK_SSMK_CONTEXT_HPP_
