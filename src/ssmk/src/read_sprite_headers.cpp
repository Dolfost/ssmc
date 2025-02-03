#include <ssmk/ssmk.hpp>
#include <ssmk/sprite.hpp>
#include <ssmk/exceptions.hpp>

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <algorithm>

#include <png.h>

namespace sm {

void ssmk::read_sprite_headers() {
	static const std::size_t sigLen = 8;
	unsigned char signature[sigLen];
	std::size_t spriteCount = context.im.sprites.size();

	std::FILE* file = nullptr;
	for (std::size_t i = 0; i < spriteCount; i++) {
		sprite* const sprt = static_cast<sprite*>(context.im.sprites[i]);
		png_structp& png  = sprt->png().image;
		png_infop&   info = sprt->png().info;

		if (not (file = std::fopen(sprt->path().c_str(), "rb")))
			SM_EX_THROW(png_error, png_failed_to_open_for_reading, sprt->path());

		std::memset(signature, 0, sigLen);
		std::fread(signature, 1, sigLen, file);
		if (not png_check_sig(signature, 8)) {
			std::fclose(file);
			SM_EX_THROW(png_error, png_bad_signature, sprt->path());
		}

		png = png_create_read_struct(
			PNG_LIBPNG_VER_STRING,
			nullptr, nullptr, nullptr
		);
		if (not png) {
			std::fclose(file);
			SM_EX_THROW(png_error, png_could_not_create_read_structure, sprt->path());
		}

		info = png_create_info_struct(png);
		if (not info) {
			std::fclose(file);
			SM_EX_THROW(png_error, png_could_not_create_info_structure, sprt->path());
		}

		png_init_io(png, file);
		png_set_sig_bytes(png, sigLen);
		png_read_info(png, info);

		png_uint_32 width = 0, height = 0;
		int depth = 0, color = 0;
		png_get_IHDR(
			png, info, 
			&width, &height, &depth, &color, 
			nullptr, nullptr, nullptr
		);

		// or's current image with all input
		context.im.color_present   += bool(color & PNG_COLOR_MASK_COLOR);
		context.im.palette_present += bool(color & PNG_COLOR_MASK_PALETTE);
		context.im.alpha_present   += bool(color & PNG_COLOR_MASK_ALPHA);
		context.im.tRNS_present    += bool(png_get_valid(png, info, PNG_INFO_tRNS));
		context.im.depth          =  std::max(context.im.depth, depth);

		sprt->setSize({width, height});
		sprt->png().pos = std::ftell(file);
		png_init_io(png, nullptr);
		std::fclose(file);

		if (m_image_header_read_callback)
			m_image_header_read_callback(m_context, i);
	}

	if (m_image_headers_read_callback)
		m_image_headers_read_callback(m_context);
}

}
