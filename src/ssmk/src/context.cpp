#include <ssmk/context.hpp>

#include <png.h>

namespace sm {

const std::unordered_map<std::string, context::output::packing::algorithm> 
	context::output::packing::algorithmText {
	{ "firstFit", context::output::packing::algorithm::FirstFit },
	{ "nextFit",  context::output::packing::algorithm::NextFit },
	{ "treeFit",  context::output::packing::algorithm::TreeFit },
};

const std::unordered_map<std::string, context::output::packing::Ordering> 
	context::output::packing::orderText {
	{ "none",       context::output::packing::Ordering::None },
	{ "decreasing", context::output::packing::Ordering::Decreasing },
	{ "increasing", context::output::packing::Ordering::Increasing },
};

const std::unordered_map<std::string, context::output::packing::SortingMetric> 
	context::output::packing::metricText {
	{ "width",     context::output::packing::SortingMetric::Width },
	{ "height",    context::output::packing::SortingMetric::Height },
	{ "maxSide",   context::output::packing::SortingMetric::MaxSide },
	{ "minSide",   context::output::packing::SortingMetric::MinSide },
	{ "perimeter", context::output::packing::SortingMetric::Perimeter },
	{ "area",      context::output::packing::SortingMetric::Area },
};

const std::unordered_map<std::string, context::output::png_info::Interlacing> 
	context::output::png_info::interlacingText {
	{ "none",     context::output::png_info::Interlacing::None },
	{ "adam7",     context::output::png_info::Interlacing::Adam7 },
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
