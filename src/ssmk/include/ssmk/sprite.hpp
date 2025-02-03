#ifndef _SSMK_SSMK_SPRITE_HPP_
#define _SSMK_SSMK_SPRITE_HPP_

#include <calgo/optim/packing2D.hpp>

#include <filesystem>
#include <iosfwd>

typedef struct png_struct_def png_struct;
typedef png_struct* png_structp;
typedef struct png_info_def png_info;
typedef png_info* png_infop;

namespace sm {

class Sprite: public ca::optim::Box2D<std::size_t> {
public:
	struct Png {
		png_structp image = nullptr;
		png_infop info = nullptr;
		std::size_t pos = 0;
		~Png();
	};
public:
	using ca::optim::Box2D<std::size_t>::Box2D;

	 void setPath(const std::filesystem::path& path) {
		s_path = path;
	}
	const std::filesystem::path& path() const {
		return s_path;
	}
	Png& png() {
		return s_png;
	}

	friend std::ostream& operator<<(std::ostream& os, const Sprite& s) {
		return os << static_cast<ca::optim::Box2D<std::size_t>>(s)
		<< " in " << s.s_path;
	};
private:
	std::filesystem::path s_path;
	Png s_png;
};

}

#endif // !_SSMK_SSMK_IMAGE_HPP_
