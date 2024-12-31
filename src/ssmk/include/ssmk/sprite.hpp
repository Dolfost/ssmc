#ifndef _SSMK_SSMK_IMAGE_HPP_
#define _SSMK_SSMK_IMAGE_HPP_

#include <calgo/optim/packing2D.hpp>

#include <filesystem>

namespace sm {

class Sprite: public ca::optim::Box2D <std::size_t> {
	using ca::optim::Box2D<std::size_t>::Box2D;

	 void setPath(const std::filesystem::path& path) {
		i_path = path;
	}
	const std::filesystem::path& path() const {
		return i_path;
	}
private:
	std::filesystem::path i_path;
};

}

#endif // !_SSMK_SSMK_IMAGE_HPP_
