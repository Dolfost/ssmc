#include <ssmk/ssmk.hpp>
#include <ssmk/exceptions.hpp>

#include <calgo/optim/packing2D.hpp>

namespace sm {

using Algorithm = context::output::packing::algorithm;
using Order = context::output::packing::ordering;
using Metric = context::output::packing::sorting_metric;

void ssmk::pack_sprites() {
	std::function<bool(const std::size_t&, const std::size_t&)> order;
	switch (context.out.pack.order) {
		case Order::decreasing:
			order = std::greater<const std::size_t&>();
			break;
		case Order::increasing:
			order = std::less<const std::size_t&>();
			break;
		case Order::none: ;
	}

	std::function<std::size_t(const ca::optim::Box2D<std::size_t>* box)> metric;
	if (order) 
		switch (context.out.pack.metric) {
			case Metric::perimeter:
				metric = [](auto box) { return box->perimeter(); };
				break;
			case Metric::min_side:
				metric = [](auto box) { return box->size().min(); };
				break;
			case Metric::max_side:
				metric = [](auto box) { return box->size().max(); };
				break;
			case Metric::width:
				metric = [](auto box) { return box->size().width(); };
				break;
			case Metric::height:
				metric = [](auto box) { return box->size().height(); };
				break;
			case Metric::area:
				metric = [](auto box) { return box->area(); };
				break;
			case Metric::none:
				SM_EX_THROW(error, no_packing_metric);
		}

	ca::optim::Packing2D<std::size_t>* packing;
	switch (context.out.pack.alg) {
		case Algorithm::tree_fit: {
			packing = new ca::optim::TreeFit2D<std::size_t>; 
			break; }
		case Algorithm::first_fit: {
			packing = new ca::optim::FirstFit2D<std::size_t>; 
			break; }
		case Algorithm::next_fit: {
			auto p = new ca::optim::NextFit2D<std::size_t>; 
			p->setK(context.out.pack.k);
			packing = p;
			break; }
		case Algorithm::none: 
			SM_EX_THROW(error, no_packing_algorithm);
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
				this->m_image_packed_callback(m_context, index);
			}
		);

	packing->pack(context.im.sprites);
	context.im.width = packing->size().width();
	context.im.height = packing->size().height();

	delete packing;

	if (m_images_packed_callback)
		m_images_packed_callback(m_context);
}

}
