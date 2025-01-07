#include <ssmk/context.hpp>

#include <png.h>

namespace sm {

const std::unordered_map<std::string, Context::Output::Packing::Algorithm> 
	Context::Output::Packing::algorithmText {
	{ "firstFit", Context::Output::Packing::Algorithm::FirstFit },
	{ "nextFit",  Context::Output::Packing::Algorithm::NextFit },
	{ "treeFit",  Context::Output::Packing::Algorithm::TreeFit },
};

const std::unordered_map<std::string, Context::Output::Packing::Order> 
	Context::Output::Packing::orderText {
	{ "none",       Context::Output::Packing::Order::None },
	{ "decreasing", Context::Output::Packing::Order::Decreasing },
	{ "increasing", Context::Output::Packing::Order::Increasing },
};

const std::unordered_map<std::string, Context::Output::Packing::Metric> 
	Context::Output::Packing::metricText {
	{ "width",     Context::Output::Packing::Metric::Width },
	{ "height",    Context::Output::Packing::Metric::Height },
	{ "maxSide",   Context::Output::Packing::Metric::MaxSide },
	{ "minSide",   Context::Output::Packing::Metric::MinSide },
	{ "perimeter", Context::Output::Packing::Metric::Perimeter },
	{ "area",      Context::Output::Packing::Metric::Area },
};

const std::unordered_map<std::string, Context::Output::Png::Interlacing> 
	Context::Output::Png::interlacingText {
	{ "none",     Context::Output::Png::Interlacing::None },
	{ "adam7",     Context::Output::Png::Interlacing::Adam7 },
};

Context::Intermediate::~Intermediate() {
	for (auto ptr: sprites)
		delete ptr;
	if (rows != nullptr)
		for (std::size_t i = 0; i < height; i++)
			delete[] ((png_bytepp)rows)[i];
	delete[] (png_bytepp)rows; 
	png_structp& p = (png_structp&)png;
	png_infop& i = (png_infop&)info;
	png_destroy_write_struct(&p, &i);
}

}
