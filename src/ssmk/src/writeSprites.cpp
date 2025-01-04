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

	const int odepth = context.im.maxBitDepth, ocolor = context.im.maxColorType;

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
		odepth, ocolor, PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT
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
	const std::size_t pixelSize = png_get_channels(opng, oinfo);

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
		png_init_io(sprite->png().image, ifile);

		int depth, color;
		png_get_IHDR(
			sprite->png().image, sprite->png().info,
			nullptr, nullptr, &depth, &color, 
			nullptr, nullptr, nullptr
		);

		// replace tRNS chunk with alpha channel
		const bool tRNS = png_get_valid(
			png, info,
			PNG_INFO_tRNS
		);
		if (tRNS) {
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
		if (odepth > depth)
			png_set_expand_16(png);

		png_read_update_info(sprite->png().image, sprite->png().info);
		int passes = png_set_interlace_handling(sprite->png().image);

		for (std::size_t p = 0; p < passes; p++)
			for (std::size_t r = 0; r < sprite->size().height(); r++)
				png_read_row(
					sprite->png().image, 
					orows[sprite->y() + r] + sprite->x()*pixelSize, 
					nullptr
				);

		png_read_end(sprite->png().image, nullptr);
		png_destroy_read_struct(&sprite->png().image, &sprite->png().info, nullptr);
		sprite->png().image = nullptr;
		sprite->png().info = nullptr;
		std::fclose(ifile);
	}

	png_write_image(opng, orows);
	png_write_end(opng, nullptr);

	// cleanup
	for (std::size_t i = 0; i < context.im.height; i++)
		png_free(opng, orows[i]);
	png_free(opng, orows);
	png_destroy_write_struct(&opng, &oinfo);
	std::fclose(ofile);
}


/*
 *  TODO:
 * - unknow chunk handling (exception)
 * - 
*/

}
