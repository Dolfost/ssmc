#include <ssmk/ssmk.hpp>
#include <ssmk/exceptions.hpp>
#include <ssmk/version.hpp>

#include <png.h>

namespace sm {

void ssmk::make_output_png() {
	png_structp& png = (png_structp&)context.im.png;
	png_infop& info = (png_infop&)context.im.info;

	context.im.color = 0;
	// expand all to rgb if required
	if (context.im.color_present)
		context.im.color |= PNG_COLOR_MASK_COLOR;
	if ((context.im.alpha_present or context.im.tRNS_present) and not 
		context.out.png.opaque)
		context.im.color |= PNG_COLOR_MASK_ALPHA;

	int interlacing;
	switch (context.out.png.inter) {
		case context::output::png_info::interlacing::none:
			interlacing = PNG_INTERLACE_NONE;
			break;
		case context::output::png_info::interlacing::adam7:
			interlacing = PNG_INTERLACE_ADAM7;
			break;
	}

	png = png_create_write_struct(
		PNG_LIBPNG_VER_STRING,
		nullptr, nullptr, nullptr
	);

	if (not png) {
		SM_EX_THROW(error, png_could_not_create_write_structure);
	}

	info = png_create_info_struct(png);
	if (not info) {
		png_destroy_write_struct(&png, nullptr);
		SM_EX_THROW(error, png_could_not_create_info_structure);
	}

	png_set_IHDR(
		png, info, context.im.width, context.im.height,
		context.im.depth, context.im.color, interlacing,
		PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT
	);

	context.im.background = new png_color_16;
	if (not context.im.background)
		SM_EX_THROW(
			png_error, 
			png_could_not_allocate_background_color, 
			context.out.file
		);
	png_color_16& bkgd = *(png_color_16p)context.im.background;
	int max = 1 << context.im.depth; // max pixel component value
	if (context.im.color & PNG_COLOR_MASK_COLOR) {
		bkgd.red =   max * context.out.png.background[0];
		bkgd.green = max * context.out.png.background[1];
		bkgd.blue =  max *context.out.png.background[2];
	} else {
		bkgd.gray = (
			6968  * max * context.out.png.background[0] + 
			23434 * max * context.out.png.background[1] + 
			2366  * max * context.out.png.background[2]
		) / 32768;
	}
	if (not (context.im.color & PNG_COLOR_MASK_ALPHA))
		png_set_bKGD(
			png, info,
			&bkgd
		);

	png_set_compression_level(
		png, context.out.png.compression
	);

	// put comments
	png_text text[2] { 
		{
			PNG_TEXT_COMPRESSION_NONE,
			(char*)version.png.key,     // sorry
			(char*)sm::version.png.text // sorry
		},
		{
			PNG_TEXT_COMPRESSION_NONE,
			(char*)version.png.versionKey,  // sorry
			(char*)sm::version.full.c_str() // sorry
		}
	};
	png_set_text(png, info, text, 2);

	// allocate result buffer
	context.im.rows = new png_bytep[context.im.height];
	png_bytepp rows = (png_bytepp&)context.im.rows;
	if (not rows) {
		png_destroy_write_struct(&png, &info);
		SM_EX_THROW(png_error, png_could_not_allocate_output_rows, context.out.file);
	}
	for (std::size_t i = 0; i < context.im.height; i++) {
		rows[i] = new png_byte[png_get_rowbytes(png, info)];
		
		if (not rows[i]) {
			png_destroy_write_struct(&png, &info);
			for (std::size_t j = 0; j < i; j++)
				delete rows[j];
			delete rows;
			rows = nullptr;
			SM_EX_THROW(png_error, png_could_not_allocate_output_rows, context.out.file);
		}
	}

}

}
