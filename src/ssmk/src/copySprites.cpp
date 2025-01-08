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

	const bool ocol  = context.im.color & PNG_COLOR_MASK_COLOR;
	const bool oalph = context.im.color & PNG_COLOR_MASK_ALPHA;
	const bool oplt  = context.im.color & PNG_COLOR_MASK_PALETTE;
	
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

		const bool col  = color & PNG_COLOR_MASK_COLOR;
		const bool alph = color & PNG_COLOR_MASK_ALPHA;
		const bool plt  = color & PNG_COLOR_MASK_PALETTE;
		const bool tRNS = png_get_valid(
			png, info,
			PNG_INFO_tRNS
		);

		// transform image to output type
		if (ocol and not col)
			png_set_gray_to_rgb(png);
		if ((depth < 8 or plt) and context.im.depth == 8)
			png_set_expand(png);
		else if (depth < 16 and context.im.depth == 16)
			png_set_expand_16(png);
		if (oalph and not alph) {
			if (tRNS)
				png_set_tRNS_to_alpha(png);
			else
				png_set_add_alpha(png, 0xFFFFFFFF, PNG_FILLER_AFTER);
		} else if ((alph or tRNS) and not oalph) {
			png_set_background(
				png, (png_color_16p)context.im.background, 
				PNG_BACKGROUND_GAMMA_SCREEN, 0, 1
			);
		}

		int passes = png_set_interlace_handling(png);

		png_read_update_info(png, info);

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

}
