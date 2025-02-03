#include <ssmk/sprite.hpp>
  
#include <png.h>

namespace sm {

sprite::png_info::~png_info() {
	png_destroy_read_struct(
		&image, &info, nullptr
	);
}

}
