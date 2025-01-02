#include <ssmk/ssmk.hpp>
#include <ssmk/sprite.hpp>
#include <ssmk/exceptions.hpp>

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <algorithm>

#include <png.h>

int maxColorType(int a, int b) {
	static const std::array<int, 5> comp = {
		PNG_COLOR_TYPE_PALETTE,
		PNG_COLOR_TYPE_GRAY,
		PNG_COLOR_TYPE_GRAY_ALPHA,
		PNG_COLOR_TYPE_RGB,
		PNG_COLOR_TYPE_RGB_ALPHA
	};
	std::size_t dist_a = std::distance(
		comp.cbegin(), 
		std::find(comp.cbegin(), comp.cend(), a)
	);
	std::size_t dist_b = std::distance(
		comp.cbegin(), 
		std::find(comp.cbegin(), comp.cend(), a)
	);
	if (dist_a > dist_b)
		return a;
	else
		return b;
}

namespace sm {

void Ssmk::readImageHeaders() {
	static const std::size_t signatureLength = 8;
	unsigned char signature[signatureLength];
	std::size_t spriteCount = context.im.sprites.size();
	std::FILE* file;
	for (std::size_t i = 0; i < spriteCount; i++) {
		Sprite& sprite = context.im.sprites[i];
		if (not (file = std::fopen(sprite.path().c_str(), "rb")))
			SM_EX_THROW(PngError, PngFailedToOpenForReading, sprite.path());

		std::memset(signature, 0, signatureLength);
		std::fread(signature, 1, signatureLength, file);
		if (!png_check_sig(signature, 8))
			SM_EX_THROW(PngError, PngBadSignature, sprite.path());

		sprite.png().image = png_create_read_struct(
			PNG_LIBPNG_VER_STRING,
			nullptr, nullptr, nullptr
		);
		if (not sprite.png().image)
			SM_EX_THROW(PngError, PngCouldNotCreateReadStructure, sprite.path());

		sprite.png().info = png_create_info_struct(
			sprite.png().image
		);
		if (not sprite.png().info)
			SM_EX_THROW(PngError, PngCouldNotCreateInfoStructure, sprite.path());

		png_init_io(sprite.png().image, file);
		png_set_sig_bytes(sprite.png().image, signatureLength);
		png_read_info(sprite.png().image, sprite.png().info);

		png_uint_32 width = 0, height = 0;
		int bitDepth = 0, colorType = 0;
		png_get_IHDR(
			sprite.png().image, sprite.png().info, 
			&width, &height, &bitDepth, &colorType, 
			nullptr, nullptr, nullptr
		);

		context.im.maxColorType = maxColorType(
			context.im.maxColorType, colorType
		);
		context.im.maxBitDepth = std::max(
			context.im.maxBitDepth, bitDepth
		);

		sprite.setSize({width, height});
		sprite.png().pos = std::ftell(file);
		std::fclose(file);
	}
}

}
