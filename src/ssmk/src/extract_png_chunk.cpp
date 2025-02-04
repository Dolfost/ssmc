#include <ssmk/ssmk.hpp>

#include <ssmk/exceptions.hpp>

#include <png.h>

#include <algorithm>
#include <cstring>

namespace sm {

void ssmk::extract_png_chunk() {
	png_unknown_chunkp chunks, chunk = nullptr;
	int count = png_get_unknown_chunks(
		context.im.png, 
		context.im.info, 
		&chunks
	 );

	// chunk = std::find(
	// 	chunks, chunks + count, 
	// 	[](auto a) {
	// 		return not std::strcmp(a, chunk_name);
	// 	}
	// );

	if (chunk == nullptr)
		SM_EX_THROW(png_error, png_no_ssmk_chunk, m_context.conf.file);

}

}
