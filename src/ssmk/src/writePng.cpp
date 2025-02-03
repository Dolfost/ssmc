#include <ssmk/ssmk.hpp>
#include <ssmk/exceptions.hpp>
#include <ssmk/version.hpp>

#include <png.h>

namespace sm {

void ssmk::write_png() {
	std::FILE* ofile = 
		std::fopen(context.output.file.c_str(), "wb");
	if (not ofile)
		SM_EX_THROW(PngError, PngFailedToOpenForWritting, context.output.file);

	png_structp& png = (png_structp&)context.im.png;
	png_infop& info = (png_infop&)context.im.info;
	png_bytepp& rows = (png_bytepp&)context.im.rows;

	png_init_io(png, ofile);

	png_write_info(png, info);

	if (m_image_row_written_callback) {
		int passes = png_set_interlace_handling(png);
		for (std::size_t p = 0; p < passes; p++)
			for (std::size_t r = 0; r < context.im.height; r++) {
				png_write_rows(
					png,
					rows,
					1
				);
				// unfortunatetly we cannot use png_set_write_status_fn...
				m_image_row_written_callback(
					*this, r, p, passes
				);
			}
	} else
		png_write_image(png, rows);

	png_write_end(png, nullptr);

	// cleanup
	for (std::size_t i = 0; i < context.im.height; i++)
		delete rows[i];
	delete rows; 
	rows = nullptr;
	delete (png_color_16p)context.im.background;
	context.im.background = nullptr;
	png_destroy_write_struct(&png, &info);
	std::fclose(ofile);

	if (m_png_written_callback) 
		m_png_written_callback(*this);
}

}
