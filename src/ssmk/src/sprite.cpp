#include <ssmk/sprite.hpp>
  
#include <png.h>

namespace sm {

Sprite::Png::~Png() {
	png_destroy_read_struct(
		&image, nullptr, nullptr
	);
}

}
