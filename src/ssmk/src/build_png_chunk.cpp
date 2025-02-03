#include <ssmk/ssmk.hpp>

#include <ssmk/sprite.hpp>

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

void ssmk::build_png_chunk() {
	m_context.im.chunk_size = sizeof(std::uint32_t);
	std::vector<std::string> paths;
	paths.reserve(m_context.im.sprites.size());
	for (const auto& p: m_context.im.sprites) {
		paths.push_back(
			std::filesystem::path(
				static_cast<sprite*>(p)->path().lexically_relative(
					m_context.conf.directory
				)
			).replace_extension("").generic_string()
		);
		m_context.im.chunk_size += paths.back().length() + 1 + 4*sizeof(std::uint32_t);
	}

	m_context.im.chunk = std::malloc(m_context.im.chunk_size);

	const bool call = (bool)m_png_chunk_entry_written_callback;
	std::size_t at = sizeof(std::uint32_t);
	std::uint32_t x, y, w, h, size, n = m_context.im.sprites.size();
	*static_cast<std::uint32_t*>(m_context.im.chunk) = htonl(n); // n
	for (std::vector<std::string>::size_type i = 0; i < paths.size(); i++) {
		const sprite* s = static_cast<sprite*>(m_context.im.sprites[i]);

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
			m_png_chunk_entry_written_callback(m_context, i);
	}

	png_unknown_chunkp chunk = (png_unknown_chunkp)std::malloc(sizeof(png_unknown_chunk));
	std::memcpy(
		chunk->name,
		chunk_name,
		5
	);
	chunk->data = (png_bytep)m_context.im.chunk;
	chunk->size = m_context.im.chunk_size;
	chunk->location = PNG_HAVE_PLTE; // write chunk before IDAT
	png_set_unknown_chunks((png_structp)m_context.im.png, (png_infop)m_context.im.info, chunk, 1);
	png_set_keep_unknown_chunks(
		(png_structp)m_context.im.png, 
		PNG_HANDLE_CHUNK_ALWAYS, 
		(png_bytep)chunk_name, 1
	);

	if (m_png_chunk_built_callback) 
		m_png_chunk_built_callback(m_context);
}

}
