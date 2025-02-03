#ifndef _SSMK_SSMK_HPP_
#define _SSMK_SSMK_HPP_

#include <ssmk/context.hpp>
#include <ssmk/sprite.hpp>

#include <functional>

#define CALLBACK(NAME, ...) \
	private: \
		std::function<void(const Ssmk& __VA_OPT__(,) __VA_ARGS__)> m_##NAME##_callback; \
	public: \
		void set_##NAME##_callback(const std::function<void(const Ssmk& __VA_OPT__(,) __VA_ARGS__)>& callback) { \
			m_##NAME##_callback = callback; \
		} \
		const std::function<void(const Ssmk& __VA_OPT__(,) __VA_ARGS__)>& NAME##_callback() const { \
			return m_##NAME##_callback; \
		} \
		std::function<void(const Ssmk& __VA_OPT__(,) __VA_ARGS__)>& NAME##_callback() { \
			return m_##NAME##_callback; \
		}

namespace sm {

class Ssmk {
public:
	Ssmk(const sm::Context& context = {}): m_context(context) {};

	sm::Context& context = m_context;

public:
	struct CallbackInfo {
		const Ssmk& ssmk;
	};

public:
	void readConfig();
	CALLBACK(config_read)

	void findFiles();
	CALLBACK(
		file_found
	)
	CALLBACK(
		files_found
	)

	void readSpriteHeaders();
	CALLBACK(
		image_header_read,
		std::size_t image
	)
	CALLBACK(
		image_headers_read,
	)

	void packSprites();
	CALLBACK(
		image_packed,
		std::size_t imageNo
	)
	CALLBACK(
		images_packed
	)

	void makeOutputPng();

	void buildPngChunk();
	CALLBACK(
		png_chunk_entry_written,
		std::size_t sprite
	)
	CALLBACK(
		png_chunk_built
	)

	void copySprites();
	CALLBACK(
		sprite_row_copied,
		std::size_t sprite,
		std::size_t row,
		std::size_t pass,
		std::size_t passes
	)
	CALLBACK(
		sprite_copied,
		std::size_t image
	)
	CALLBACK(
		sprites_copied
	)

	void writePng();
	CALLBACK(
		image_row_written,
		std::size_t row,
		std::size_t pass,
		std::size_t passes
	)
	CALLBACK(
		png_written
	)

public:
	void operator()() {
		readConfig();
		findFiles();
		readSpriteHeaders();
		packSprites();
		makeOutputPng();
		copySprites();
		buildPngChunk();
		writePng();
	}

	static void fillContext(sm::Context& context);

public:
	constexpr static const std::array configFilenames = {
		"ssmk.toml", "sprite.toml", "spritesheet.toml"
	};
	constexpr static const char* chunk_name = "ssMK";

private:
	sm::Context m_context;
};

#undef CALLBACK

}

#endif // !_SSMK_SSMK_HPP_
