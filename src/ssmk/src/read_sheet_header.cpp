#include <ssmk/ssmk.hpp>

#include <ssmk/exceptions.hpp>

#include <cstdio>
#include <cstring>

#include <png.h>

namespace sm {

void ssmk::read_sheet_header() {
	static const std::size_t sigLen = 8;
	png_byte signature[sigLen];
	std::memset(signature, 0, sigLen);

	std::FILE* file = nullptr;
	if (not (file = std::fopen(m_context.conf.file.c_str(), "rb")))
		SM_EX_THROW(png_error, png_failed_to_open_for_reading, m_context.conf.file);

	std::fread(signature, 1, sigLen, file);
	if (not png_check_sig(signature, 8)) {
		std::fclose(file);
		SM_EX_THROW(png_error, png_bad_signature, m_context.conf.file);
	}

	context.im.png = png_create_read_struct(
		PNG_LIBPNG_VER_STRING,
		nullptr, nullptr, nullptr
	);

	if (not context.im.png) {
		SM_EX_THROW(error, png_could_not_create_read_structure);
	}

	context.im.info = png_create_info_struct(context.im.png);
	if (not context.im.info) {
		png_destroy_read_struct(&context.im.png, nullptr, nullptr);
		SM_EX_THROW(error, png_could_not_create_info_structure);
	}

	png_set_keep_unknown_chunks(
		context.im.png, 
		PNG_HANDLE_CHUNK_ALWAYS, 
		(const png_bytep)chunk_name, 
		1
	);

	png_init_io(m_context.im.png, file);
	png_set_sig_bytes(m_context.im.png, sigLen);
	png_read_info(m_context.im.png, m_context.im.info);

	png_uint_32 width = 0, height = 0;
	int depth = 0, color = 0;
	png_get_IHDR(
		m_context.im.png, m_context.im.info, 
		&width, &height, &depth, &color, 
		nullptr, nullptr, nullptr
	);

	// or's current image with all input
	context.im.color_present   = bool(color & PNG_COLOR_MASK_COLOR);
	context.im.palette_present = bool(color & PNG_COLOR_MASK_PALETTE);
	context.im.alpha_present   = bool(color & PNG_COLOR_MASK_ALPHA);
	context.im.tRNS_present    = bool(png_get_valid(m_context.im.png, m_context.im.info, PNG_INFO_tRNS));
	context.im.depth           = depth;
	context.im.color           = color;

	if (m_sheet_header_read_callback)
		m_sheet_header_read_callback(m_context);
}

}
