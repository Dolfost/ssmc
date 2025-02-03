#include <ssmk/ssmk.hpp>

#include <cstdlib>
#include <cstddef>
#include <cstring>
#include <cstdint>

#include <png.h>

#ifdef _WIN32
#include <winsock.h>
#else
#include <arpa/inet.h>
#endif

namespace sm {

void Ssmk::buildPngChunk() {
	m_context.im.chunkSize = sizeof(std::uint32_t);
	std::vector<std::string> paths;
	paths.reserve(m_context.im.sprites.size());
	for (const auto& p: m_context.im.sprites) {
		paths.push_back(
			std::filesystem::path(
				static_cast<Sprite*>(p)->path().lexically_relative(
					m_context.config.directory
				)
			).replace_extension("").generic_string()
		);
		m_context.im.chunkSize += paths.back().length() + 1 + 4*sizeof(std::uint32_t);
	}

	m_context.im.chunk = std::malloc(m_context.im.chunkSize);

	const bool call = (bool)m_png_chunk_entry_written_callback;
	std::size_t at = sizeof(std::uint32_t);
	std::uint32_t x, y, w, h, size, n = m_context.im.sprites.size();
	*static_cast<std::uint32_t*>(m_context.im.chunk) = htonl(n); // n
	for (std::vector<std::string>::size_type i = 0; i < paths.size(); i++) {
		const Sprite* s = static_cast<Sprite*>(m_context.im.sprites[i]);

		x = htonl(s->x()), y = htonl(s->y()), w = htonl(s->size().width()), h = htonl(s->size().height());
		std::memcpy( // x
			static_cast<std::uint8_t*>(m_context.im.chunk) + at, 
			&x, sizeof(std::uint32_t)
		);
		at += sizeof(std::uint32_t);
		std::memcpy( // y
			static_cast<std::uint8_t*>(m_context.im.chunk) + at, 
			&y, sizeof(std::uint32_t)
		);
		at += sizeof(std::uint32_t);
		std::memcpy( // w
			static_cast<std::uint8_t*>(m_context.im.chunk) + at, 
			&w, sizeof(std::uint32_t)
		);
		at += sizeof(std::uint32_t);
		std::memcpy( // h
			static_cast<std::uint8_t*>(m_context.im.chunk) + at, 
			&h, sizeof(std::uint32_t)
		);
		at += sizeof(std::uint32_t);

		size = paths[i].length() + 1;

		std::memcpy(
			static_cast<char*>(m_context.im.chunk) + at, 
			paths[i].c_str(),
			size
		);
		at += size;
		
		if (call)
			m_png_chunk_entry_written_callback(*this, i);
	}

	png_unknown_chunkp chunk = (png_unknown_chunkp)std::malloc(sizeof(png_unknown_chunk));
	std::memcpy(
		chunk->name,
		chunk_name,
		5
	);
	chunk->data = (png_bytep)m_context.im.chunk;
	chunk->size = m_context.im.chunkSize;
	chunk->location = PNG_HAVE_PLTE; // write chunk before IDAT
	png_set_unknown_chunks((png_structp)m_context.im.png, (png_infop)m_context.im.info, chunk, 1);
	png_set_keep_unknown_chunks(
		(png_structp)m_context.im.png, 
		PNG_HANDLE_CHUNK_ALWAYS, 
		(png_bytep)chunk_name, 1
	);

	if (m_png_chunk_built_callback) 
		m_png_chunk_built_callback(*this);
}

}
