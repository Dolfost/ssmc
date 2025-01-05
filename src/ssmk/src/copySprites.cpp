#include <ssmk/ssmk.hpp>
#include <ssmk/exceptions.hpp>
#include <ssmk/version.hpp>

#include <cstdio>
#include <cstring>

#include <zlib.h>
#include <png.h>

namespace sm {

void Ssmk::copySprites() {
	png_structp& opng = (png_structp&)context.im.png;
	png_infop& oinfo = (png_infop&)context.im.info;

	const bool ocol = context.im.color  & PNG_COLOR_MASK_COLOR;
	const bool oalph = context.im.color & PNG_COLOR_MASK_ALPHA;
	
	const std::size_t pixelSize = // some shifting magic :)
		png_get_channels(opng, oinfo) << (context.im.depth >> 4);

	std::size_t spriteCount = context.im.sprites.size();
	std::FILE* ifile = nullptr;
	for (int i = 0; i < spriteCount; i++) {
		Sprite* const sprite = static_cast<Sprite*>(context.im.sprites[i]);
		png_infop&   info = sprite->png().info;
		png_structp& png = sprite->png().image;

		ifile = std::fopen(sprite->path().c_str(), "rb");
		if (not ifile)
			SM_EX_THROW(PngError, PngFailedToOpenForReading, sprite->path());
		std::fseek(ifile, sprite->png().pos, SEEK_SET);
		png_init_io(png, ifile);

		int depth, color;
		png_get_IHDR(
			png, info,
			nullptr, nullptr, &depth, &color, 
			nullptr, nullptr, nullptr
		);

		// replace tRNS chunk with alpha channel
		const bool tRNS = png_get_valid(
			png, info,
			PNG_INFO_tRNS
		);
		if (tRNS and oalph) {
			png_set_tRNS_to_alpha(png);
			color |= PNG_COLOR_MASK_ALPHA;
		}

		const bool col = color & PNG_COLOR_MASK_COLOR;
		const bool alph = color & PNG_COLOR_MASK_ALPHA;

		// transform image to output type
		if (ocol and not col) {
			if (depth < 8)
				png_set_expand_gray_1_2_4_to_8(png);
			png_set_gray_to_rgb(png);
		}
		if (oalph and not alph)
			png_set_add_alpha(png, 1 << 16, PNG_FILLER_AFTER);
		if (alph and not oalph)
			png_set_background(
				png, (png_color_16p)context.im.background, 
				PNG_BACKGROUND_GAMMA_SCREEN, 0, 1
			);
		if (context.im.depth > depth)
			png_set_expand_16(png);

		int passes = png_set_interlace_handling(png);

		png_read_update_info(sprite->png().image, sprite->png().info);

		png_bytepp rows = (png_bytepp)context.im.rows;
		if (s_spriteRowCopiedCallback) {
			for (std::size_t p = 0; p < passes; p++)
				for (std::size_t r = 0; r < sprite->size().height(); r++) {
					png_read_row(
						png,
						rows[sprite->y() + r] + sprite->x()*pixelSize, 
						nullptr
					);
					// unfortunatetly we cannot use png_set_read_status_fn...
					s_spriteRowCopiedCallback(
						*this, i, r, p, passes
					);
				}
		} else {
			for (std::size_t p = 0; p < passes; p++)
				for (std::size_t r = 0; r < sprite->size().height(); r++)
					png_read_row(
						png,
						rows[sprite->y() + r] + sprite->x()*pixelSize, 
						nullptr
					);
		}

		if (s_spriteCopiedCallback) 
			s_spriteCopiedCallback(*this, i);

		png_read_end(png, nullptr);
		png_destroy_read_struct(&png, &info, nullptr);
		png = nullptr;
		info = nullptr;
		std::fclose(ifile);
	}

	if (s_spritesCopiedCallback) 
		s_spritesCopiedCallback(*this);
}


/*
 *  TODO:
 * - unknow chunk handling (exception)
 * - 
*/

}
