#include <ssmk/ssmk.hpp>
#include <ssmk/exceptions.hpp>

#include <calgo/optim/packing2D.hpp>

namespace sm {

void Ssmk::packImages() {
	ca::optim::TreeFit2D<std::size_t> tf;
	if (s_imagePackedCallback)
		tf.boxPackedCallback(
			[this](
				const std::vector<ca::optim::Box2D<std::size_t>*>& boxes, 
				std::size_t index) {
				this->s_imagePackedCallback(*this, index);
			}
		);

	std::vector<ca::optim::Box2D<std::size_t>*> trans(
		context.im.sprites.begin(),
		context.im.sprites.end()
	);

	tf.pack(trans);

	if (s_imagesPackedCallback)
		s_imagesPackedCallback(*this);
}

}
