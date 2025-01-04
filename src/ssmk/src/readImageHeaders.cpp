#include <ssmk/ssmk.hpp>
#include <ssmk/sprite.hpp>
#include <ssmk/exceptions.hpp>

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <algorithm>

#include <png.h>

namespace sm {

void Ssmk::readImageHeaders() {
	static const std::size_t signatureLength = 8;
	unsigned char signature[signatureLength];
	std::size_t spriteCount = context.im.sprites.size();
	std::FILE* file;
	for (std::size_t i = 0; i < spriteCount; i++) {
		Sprite* const sprite = static_cast<Sprite*>(context.im.sprites[i]);

		if (not (file = std::fopen(sprite->path().c_str(), "rb")))
			SM_EX_THROW(PngError, PngFailedToOpenForReading, sprite->path());

		std::memset(signature, 0, signatureLength);
		std::fread(signature, 1, signatureLength, file);
		if (!png_check_sig(signature, 8)) {
			std::fclose(file);
			SM_EX_THROW(PngError, PngBadSignature, sprite->path());
		}

		sprite->png().image = png_create_read_struct(
			PNG_LIBPNG_VER_STRING,
			nullptr, nullptr, nullptr
		);
		if (not sprite->png().image) {
			std::fclose(file);
			SM_EX_THROW(PngError, PngCouldNotCreateReadStructure, sprite->path());
		}

		sprite->png().info = png_create_info_struct(
			sprite->png().image
		);
		if (not sprite->png().info) {
			std::fclose(file);
			SM_EX_THROW(PngError, PngCouldNotCreateInfoStructure, sprite->path());
		}

		png_init_io(sprite->png().image, file);
		png_set_sig_bytes(sprite->png().image, signatureLength);
		png_read_info(sprite->png().image, sprite->png().info);

		png_uint_32 width = 0, height = 0;
		int bitDepth = 0, colorType = 0;
		png_get_IHDR(
			sprite->png().image, sprite->png().info, 
			&width, &height, &bitDepth, &colorType, 
			nullptr, nullptr, nullptr
		);

		// skip paletted images
		if (colorType == PNG_COLOR_TYPE_PALETTE) {
			std::fclose(file);
			SM_EX_THROW(PngError, PngPalettedImage, sprite->path());
		}
		// skip grayscale images which color depth is < 8
		if (bitDepth < 8 and not (colorType & PNG_COLOR_MASK_COLOR)) {
			std::fclose(file);
			SM_EX_THROW(PngError, PngSub8BitGrayscaleImage, sprite->path());
		}

		const bool tRNS = png_get_valid(
			sprite->png().image, sprite->png().info,
			PNG_INFO_tRNS
		);
		if (tRNS)
			colorType |= PNG_COLOR_MASK_ALPHA;

		context.im.maxColorType |= colorType;
		context.im.maxBitDepth = std::max(
			context.im.maxBitDepth, bitDepth
		);

		sprite->setSize({width, height});
		sprite->png().pos = std::ftell(file);
		png_init_io(sprite->png().image, nullptr);
		std::fclose(file);

		if (s_imageHeaderReadCallback)
			s_imageHeaderReadCallback(*this, i);
	}

	if (s_imageHeadersReadCallback)
		s_imageHeadersReadCallback(*this);
}

}
