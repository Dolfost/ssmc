#include <cstdio>
#include <random>

#include <png.h>

int main(int argc, char** argv) {
	png_structp png;
	png_infop info;

	std::random_device rnd;
	std::uniform_int_distribution<int> side(50, 300);
	std::uniform_int_distribution<int> color(35, 255);
	for (int i = 0; i < 1000; i++) {
		int width = side(rnd), height = side(rnd);
		png_color col1, col2;
		col1.red = color(rnd);
		col1.green = color(rnd);
		col1.blue = color(rnd);
		col2.red = color(rnd);
		col2.green = color(rnd);
		col2.blue = color(rnd);
		png =  png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
		info = png_create_info_struct(png);
		png_set_IHDR(
			png, info, width, height, 8, PNG_COLOR_TYPE_RGB, 
			PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, 
			PNG_FILTER_TYPE_DEFAULT
		);
		std::FILE* fp = std::fopen((std::string("pnggen_").append(std::to_string(i)).append(".png")).c_str(), "wb");
		png_bytepp rows = new png_bytep[height];
		for (int i = 0; i < height; i++) {
			rows[i] = new png_byte[width*3];
			for (int j = 0; j < width; j++) {
				rows[i][3*j + 0] = col1.red;
				rows[i][3*j + 1] = col1.green;
				rows[i][3*j + 2] = col1.blue;
			}
		}

		for (int i = 0; i < width; i++) {
			rows[0][3*i + 0] = col2.red;
			rows[0][3*i + 1] = col2.green;
			rows[0][3*i + 2] = col2.blue;
			rows[height - 1][3*i + 0] = col2.red;
			rows[height - 1][3*i + 1] = col2.green;
			rows[height - 1][3*i + 2] = col2.blue;
		}
		for (int i = 0; i < height; i++) {
			rows[i][0] = col2.red;
			rows[i][1] = col2.green;
			rows[i][2] = col2.blue;
			rows[i][3*(width-1) + 0] = col2.red;
			rows[i][3*(width-1) + 1] = col2.green;
			rows[i][3*(width-1) + 2] = col2.blue;
		}

		png_init_io(png, fp);
		png_write_info(png, info);
		png_write_image(png, rows);
		png_write_end(png, nullptr);
		png_destroy_write_struct(&png, &info);

		for (int i = 0; i < height; i++)
			delete[] rows[i];
		delete[] rows;
	}
	
	return 0;
}
