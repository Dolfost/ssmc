#include <ssmk/ssmk.hpp>
#include <ssmk/exceptions.hpp>

#include <cstdio>
#include <cstring>

#include <zlib.h>
#include <png.h>

namespace sm {

void Ssmk::writeSprites() {
	png_structp opng = nullptr;
	png_infop oinfo = nullptr;

	const int odepth = context.im.maxBitDepth;
	const int ocolor = (context.im.maxColor ? PNG_COLOR_MASK_COLOR : 0) | 
		(context.output.png.opaque ? 0 : (context.im.maxAlpha ? PNG_COLOR_MASK_ALPHA : 0));
	int interlacing;
	switch (context.output.png.interlacing) {
		case Context::Output::Png::Interlacing::None:
			interlacing = PNG_INTERLACE_NONE;
			break;
		case Context::Output::Png::Interlacing::Adam7:
			interlacing = PNG_INTERLACE_ADAM7;
			break;
	}

	opng = png_create_write_struct(
		PNG_LIBPNG_VER_STRING,
		nullptr, nullptr, nullptr
	);

	if (not opng) {
		SM_EX_THROW(Error, PngCouldNotCreateWriteStructure);
	}

	oinfo = png_create_info_struct(opng);
	if (not oinfo) {
		png_destroy_write_struct(&opng, nullptr);
		SM_EX_THROW(Error, PngCouldNotCreateInfoStructure);
	}

	png_set_IHDR(
		opng, oinfo, context.im.width, context.im.height,
		odepth, ocolor, interlacing,
		PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT
	);

	png_color_16 bkgd;
	int max = 1 << odepth; // max pixel component value
	if (ocolor & PNG_COLOR_MASK_COLOR) {
		bkgd.red =   max*context.output.png.background[0];
		bkgd.green = max*context.output.png.background[1];
		bkgd.blue =  max*context.output.png.background[2];
	} else {
		bkgd.gray = (
			6968 *  max*context.output.png.background[0] + 
			23434 * max*context.output.png.background[1] + 
			2366 *  max*context.output.png.background[2]
		) / 32768;
	}
	if (not (ocolor & PNG_COLOR_MASK_ALPHA))
		png_set_bKGD(
			opng, oinfo,
			&bkgd
		);

	png_set_compression_level(
		opng, context.output.png.compression
	);

	std::FILE* ofile = 
		std::fopen(context.output.file.c_str(), "wb");
	if (not ofile)
		SM_EX_THROW(PngError, PngFailedToOpenForWritting, context.output.file);

	png_init_io(opng, ofile);
	png_write_info(opng, oinfo);
	png_set_packing(opng);

	// allocate result buffer
	png_bytepp orows = (png_bytepp)png_malloc(
		opng, context.im.height*sizeof(png_bytep)
	);
	if (not orows) {
		std::fclose(ofile);
		png_destroy_write_struct(&opng, &oinfo);
		SM_EX_THROW(PngError, PngCouldNotAllocateOutputRows, context.output.file);
	}
	for (std::size_t i = 0; i < context.im.height; i++) {
		orows[i] = (png_bytep)png_malloc(
			opng, png_get_rowbytes(opng, oinfo)
		);
		
		if (not orows[i]) {
			std::fclose(ofile);
			png_destroy_write_struct(&opng, &oinfo);
			for (std::size_t j = 0; j < i; j++)
				png_free(opng, orows[j]);
			png_free(opng, orows);
			SM_EX_THROW(PngError, PngCouldNotAllocateOutputRows, context.output.file);
		}
	}

	const bool ocol = ocolor & PNG_COLOR_MASK_COLOR;
	const bool oalph = ocolor & PNG_COLOR_MASK_ALPHA;
	// number of cnannels * 2 if depth if 16 or * 1 if 8
	const std::size_t pixelSize = png_get_channels(opng, oinfo) << (odepth >> 4);

	std::size_t spriteCount = context.im.sprites.size();
	std::FILE* ifile = nullptr;
	for (int i = 0; i < spriteCount; i++) {
		Sprite* const sprite = static_cast<Sprite*>(context.im.sprites[i]);
		png_infop const info = sprite->png().info;
		png_structp const png = sprite->png().image;

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
		if (ocol and not col)
			png_set_gray_to_rgb(png);
		if (oalph and not alph)
			png_set_add_alpha(png, 1 << 16, PNG_FILLER_AFTER);
		if (alph and not oalph)
			png_set_background(
				png, &bkgd, 
				PNG_BACKGROUND_GAMMA_SCREEN, 0, 1
			);
		if (odepth > depth)
			png_set_expand_16(png);

		int passes = png_set_interlace_handling(png);

		png_read_update_info(sprite->png().image, sprite->png().info);

		if (s_spriteRowCopiedCallback) {
			for (std::size_t p = 0; p < passes; p++)
				for (std::size_t r = 0; r < sprite->size().height(); r++) {
					png_read_row(
						png,
						orows[sprite->y() + r] + sprite->x()*pixelSize, 
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
						orows[sprite->y() + r] + sprite->x()*pixelSize, 
						nullptr
					);
		}

		if (s_spriteCopiedCallback) 
			s_spriteCopiedCallback(*this, i);

		png_read_end(png, nullptr);
		png_destroy_read_struct(&sprite->png().image, &sprite->png().info, nullptr);
		sprite->png().image = nullptr;
		sprite->png().info = nullptr;
		std::fclose(ifile);
	}

	if (s_spritesCopiedCallback) 
		s_spritesCopiedCallback(*this);

	if (s_imageRowWrittenCallback) {
		int passes = png_set_interlace_handling(opng);
		for (std::size_t p = 0; p < passes; p++)
			for (std::size_t r = 0; r < context.im.height; r++) {
				png_write_rows(
					opng,
					orows,
					1
				);
				// unfortunatetly we cannot use png_set_write_status_fn...
				s_imageRowWrittenCallback(
					*this, r, p, passes
				);
			}
	} else
		png_write_image(opng, orows);
	png_write_end(opng, nullptr);

	// cleanup
	for (std::size_t i = 0; i < context.im.height; i++)
		png_free(opng, orows[i]);
	png_free(opng, orows);
	png_destroy_write_struct(&opng, &oinfo);
	std::fclose(ofile);

	if (s_imageWrittenCallback) 
		s_imageWrittenCallback(*this);
}


/*
 *  TODO:
 * - unknow chunk handling (exception)
 * - 
*/

}
