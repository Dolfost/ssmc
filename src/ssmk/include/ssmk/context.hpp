#ifndef _SSMK_SSMK_CONTEXT_HPP_
#define _SSMK_SSMK_CONTEXT_HPP_

#include <vector>
#include <array>
#include <iosfwd>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <cstdint>

namespace ca::optim {

template<typename T> 
class Box2D;

}

typedef struct png_struct_def png_struct;
typedef png_struct* png_structp;
typedef struct png_info_def png_info;
typedef png_info* png_infop;
typedef struct png_color_16_struct png_color_16;
typedef png_color_16_struct* png_color_16p;
typedef unsigned char png_byte;
typedef png_byte** png_bytepp;

namespace sm {

struct context {
	struct config {
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
				none,
				first_fit,
				next_fit,
				tree_fit,
			} alg = algorithm::tree_fit;
			const static std::unordered_map<std::string, algorithm> algorithm_text;
			enum class ordering {
				none,
				decreasing,
				increasing
			} order = ordering::decreasing;
			const static std::unordered_map<std::string, ordering> order_text;
			enum class sorting_metric {
				none,
				width,
				height,
				max_side,
				min_side,
				perimeter,
				area,
			} metric = sorting_metric::min_side;
			const static std::unordered_map<std::string, sorting_metric> metric_text;
			std::size_t k = 1;
		} pack;
		struct png_info {
			bool opaque = false;
			enum class interlacing {
				none, 
				adam7,
			} inter = interlacing::none;
			const static std::unordered_map<std::string, interlacing> interlacing_text;
			std::array<double, 3> background = {0, 0, 0};
			int compression = -1;
		} png;
	} out;

	struct intermediate {
		std::vector<ca::optim::Box2D<std::size_t>*> sprites;

		std::size_t color_present   = 0;
		std::size_t alpha_present   = 0;
		std::size_t palette_present = 0;
		std::size_t tRNS_present    = 0;

		std::size_t width, height; ///< Output dimentions
		png_structp png = nullptr;  ///< Output png data structure
		png_infop info = nullptr; ///< Output png info structure
		png_color_16p background = nullptr; ///< Output png background structure
		png_bytepp rows = nullptr; ///< Output buffer
		int color = 0; ///< Output color mode
		int depth = 0; ///< Output color depth
		void* chunk = nullptr; ///< SSMK specific png chunk
		std::uint32_t chunk_size = 0;
		~intermediate();
	} im;

	friend std::ostream& operator<<(std::ostream& os, const context& c);

};

}

#endif // !_SSMK_SSMK_CONTEXT_HPP_
