#include <ssmk/context.hpp>

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
}
