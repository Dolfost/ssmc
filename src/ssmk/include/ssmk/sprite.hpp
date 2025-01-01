#ifndef _SSMK_SSMK_SPRITE_HPP_
#define _SSMK_SSMK_SPRITE_HPP_

#include <calgo/optim/packing2D.hpp>

#include <filesystem>
#include <ostream>

namespace sm {

class Sprite: public ca::optim::Box2D<std::size_t> {
public:
	using ca::optim::Box2D<std::size_t>::Box2D;

	 void setPath(const std::filesystem::path& path) {
		i_path = path;
	}
	const std::filesystem::path& path() const {
		return i_path;
	}

	friend std::ostream& operator<<(std::ostream& os, const Sprite& s) {
		return os << static_cast<ca::optim::Box2D<std::size_t>>(s)
		<< " in " << s.i_path;
	};
private:
	std::filesystem::path i_path;
};

}

#endif // !_SSMK_SSMK_IMAGE_HPP_
