#include <ssmk/ssmk.hpp>
#include <ssmk/exceptions.hpp>
#include <ssmk/version.hpp>

#include <png.h>

namespace sm {

void ssmk::write_png() {
	std::FILE* ofile = 
		std::fopen(context.out.file.c_str(), "wb");
	if (not ofile)
		SM_EX_THROW(png_error, png_failed_to_open_for_writting, context.out.file);

	png_byte b;
	png_bytepp p;
	png_init_io(context.im.png, ofile);

	png_write_info(context.im.png, context.im.info);

	if (m_image_row_written_callback) {
		int passes = png_set_interlace_handling(context.im.png);
		for (std::size_t p = 0; p < passes; p++)
			for (std::size_t r = 0; r < context.im.height; r++) {
				png_write_rows(
					context.im.png,
					context.im.rows,
					1
				);
				// unfortunatetly we cannot use png_set_write_status_fn...
				m_image_row_written_callback(
					m_context, r, p, passes
				);
			}
	} else
		png_write_image(context.im.png, context.im.rows);

	png_write_end(context.im.png, nullptr);

	// cleanup
	for (std::size_t i = 0; i < context.im.height; i++)
		delete context.im.rows[i];
	delete context.im.rows; 
	context.im.rows = nullptr;
	delete (png_color_16p)context.im.background;
	context.im.background = nullptr;
	png_destroy_write_struct(&context.im.png, &context.im.info);
	std::fclose(ofile);

	if (m_png_written_callback) 
		m_png_written_callback(m_context);
}

}
