#include <ssmk/ssmk.hpp>
#include <ssmk/exceptions.hpp>

#include <calgo/optim/packing2D.hpp>

namespace sm {

using Algorithm = Context::Output::Packing::Algorithm;
using Order = Context::Output::Packing::Order;
using Metric = Context::Output::Packing::Metric;

void ssmk::pack_sprites() {
	std::function<bool(const std::size_t&, const std::size_t&)> order;
	switch (context.output.packing.order) {
		case Order::Decreasing:
			order = std::greater<const std::size_t&>();
			break;
		case Order::Increasing:
			order = std::less<const std::size_t&>();
			break;
		case Order::None: ;
	}

	std::function<std::size_t(const ca::optim::Box2D<std::size_t>* box)> metric;
	if (order) 
		switch (context.output.packing.metric) {
			case Metric::Perimeter:
				metric = [](auto box) { return box->perimeter(); };
				break;
			case Metric::MinSide:
				metric = [](auto box) { return box->size().min(); };
				break;
			case Metric::MaxSide:
				metric = [](auto box) { return box->size().max(); };
				break;
			case Metric::Width:
				metric = [](auto box) { return box->size().width(); };
				break;
			case Metric::Height:
				metric = [](auto box) { return box->size().height(); };
				break;
			case Metric::Area:
				metric = [](auto box) { return box->area(); };
				break;
			case Metric::None:
				SM_EX_THROW(Error, NoPackingMetric);
		}

	ca::optim::Packing2D<std::size_t>* packing;
	switch (context.output.packing.algorithm) {
		case Algorithm::TreeFit: {
			packing = new ca::optim::TreeFit2D<std::size_t>; 
			break; }
		case Algorithm::FirstFit: {
			packing = new ca::optim::FirstFit2D<std::size_t>; 
			break; }
		case Algorithm::NextFit: {
			auto p = new ca::optim::NextFit2D<std::size_t>; 
			p->setK(context.output.packing.k);
			packing = p;
			break; }
		case Algorithm::None: 
			SM_EX_THROW(Error, NoPackingAlgorithm);
	}

	if (order) {
		packing->setComparator(
			[&order, &metric](auto a, auto b) {
				return order(metric(a), metric(b));
			}
		);
	}

	if (m_image_packed_callback)
		packing->boxPackedCallback(
			[this](
				const std::vector<ca::optim::Box2D<std::size_t>*>& boxes, 
				std::size_t index) {
				this->m_image_packed_callback(*this, index);
			}
		);

	packing->pack(context.im.sprites);
	context.im.width = packing->size().width();
	context.im.height = packing->size().height();

	delete packing;

	if (m_images_packed_callback)
		m_images_packed_callback(*this);
}

}
