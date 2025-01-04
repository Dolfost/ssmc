#include <ssmk/ssmk.hpp>
#include <ssmk/exceptions.hpp>

#include <cstdio>
#include <cstring>

#include <zlib.h>
#include <png.h>

namespace sm {

void Ssmk::writeImages() {
	png_structp png;
	png_infop info;

	std::FILE* ofile = 
		std::fopen(context.output.file.c_str(), "wb");
	if (not ofile)
		SM_EX_THROW(PngError, PngFailedToOpenForWritting, context.output.file);

	png = png_create_write_struct(
		PNG_LIBPNG_VER_STRING,
		nullptr, nullptr, nullptr
	);

	if (not png)
		SM_EX_THROW(Error, PngCouldNotCreateWriteStructure);

	info = png_create_info_struct(png);
	if (not info) {
		png_destroy_write_struct(&png, nullptr);
		SM_EX_THROW(Error, PngCouldNotCreateInfoStructure);
	}
	png_set_IHDR(
		png, info, context.im.width, context.im.height,
		context.im.maxBitDepth, context.im.maxColorType, PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT
	);
	png_init_io(png, ofile);
	png_write_info(png, info);
	png_set_packing(png);

	png_bytepp orows = (png_bytepp)png_malloc(
		png,
		context.im.height*sizeof(png_bytep)
	);
	for (int i = 0; i < context.im.height; i++) {
		orows[i] = (png_bytep)png_malloc(
			png, 
			png_get_rowbytes(png, info)
		);
	}

	std::size_t spriteCount = context.im.sprites.size();
	std::FILE* ifile;
	for (int i = 0; i < spriteCount; i++) {
		Sprite* sprite = static_cast<Sprite*>(context.im.sprites[i]);
		ifile = std::fopen(sprite->path().c_str(), "rb");
		if (not ifile)
			SM_EX_THROW(PngError, PngFailedToOpenForReading, sprite->path());
		std::fseek(ifile, sprite->png().pos, SEEK_SET);
		png_init_io(sprite->png().image, ifile);
		int color, depth;
		png_get_IHDR(
			sprite->png().image, sprite->png().info,
			nullptr, nullptr, &depth, &color, 
			nullptr, nullptr, nullptr
		);

		switch (context.im.maxColorType) {
			case PNG_COLOR_TYPE_RGB:
			case PNG_COLOR_TYPE_RGBA: {
				switch (color) {
					case PNG_COLOR_TYPE_PALETTE:
						png_set_palette_to_rgb(sprite->png().image);
						break;
					case PNG_COLOR_TYPE_GRAY:
					case PNG_COLOR_TYPE_GRAY_ALPHA:
						png_set_gray_to_rgb(sprite->png().image);
						break;
				}
				break;
			}
		}

		if (context.im.maxBitDepth != depth) {
			if (depth > context.im.maxBitDepth)
				png_set_scale_16(sprite->png().image);
			else 
				png_set_expand_16(sprite->png().image);
		}

    if (png_get_valid(sprite->png().image, sprite->png().info, PNG_INFO_tRNS))
       png_set_tRNS_to_alpha(sprite->png().image);

		png_read_update_info(sprite->png().image, sprite->png().info);
		int passes = png_set_interlace_handling(sprite->png().image);
		std::size_t rowbytes = png_get_rowbytes(
			sprite->png().image,
			sprite->png().info
		);
		png_bytep row = (png_bytep)png_malloc(
			sprite->png().image, 
			rowbytes
		);
		std::size_t psize = rowbytes/sprite->size().width();
		for (std::size_t p = 0; p < passes; p++) {
			for (std::size_t r = 0; r < sprite->size().height(); r++) {
				png_read_row(sprite->png().image, row, nullptr);
				std::memcpy(
					orows[sprite->y() + r] + sprite->x()*psize,
					row,
					rowbytes
				);
			}
		}
		png_free(sprite->png().image, row);
		png_read_end(sprite->png().image, nullptr);
		png_destroy_read_struct(&sprite->png().image, &sprite->png().info, nullptr);
		std::fclose(ifile);
	}

	png_write_image(png, orows);
	png_write_end(png, nullptr);
	png_destroy_write_struct(&png, &info);
	std::fclose(ofile);
}

}
