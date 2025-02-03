#include <ssmk/context.hpp>

#include <png.h>

namespace sm {

const std::unordered_map<std::string, context::output::packing::algorithm> 
	context::output::packing::algorithm_text {
	{ "firstFit", context::output::packing::algorithm::first_fit },
	{ "nextFit",  context::output::packing::algorithm::next_fit },
	{ "treeFit",  context::output::packing::algorithm::tree_fit },
};

const std::unordered_map<std::string, context::output::packing::ordering> 
	context::output::packing::order_text {
	{ "none",       context::output::packing::ordering::none },
	{ "decreasing", context::output::packing::ordering::decreasing },
	{ "increasing", context::output::packing::ordering::increasing },
};

const std::unordered_map<std::string, context::output::packing::sorting_metric> 
	context::output::packing::metric_text {
	{ "width",     context::output::packing::sorting_metric::width },
	{ "height",    context::output::packing::sorting_metric::height },
	{ "maxSide",   context::output::packing::sorting_metric::max_side },
	{ "minSide",   context::output::packing::sorting_metric::min_side },
	{ "perimeter", context::output::packing::sorting_metric::perimeter },
	{ "area",      context::output::packing::sorting_metric::area },
};

const std::unordered_map<std::string, context::output::png_info::interlacing> 
	context::output::png_info::interlacing_text {
	{ "none",     context::output::png_info::interlacing::none },
	{ "adam7",     context::output::png_info::interlacing::adam7 },
};

context::intermediate::~intermediate() {
	for (auto ptr: sprites)
		delete ptr;
	sprites.clear();
	if (rows != nullptr)
		for (std::size_t i = 0; i < height; i++)
			delete[] ((png_bytepp)rows)[i];
	delete[] (png_bytepp)rows; 
	std::free(chunk);
	png_structp& p = (png_structp&)png;
	png_infop& i = (png_infop&)info;
	png_destroy_write_struct(&p, &i);
}

}
