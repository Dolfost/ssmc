#ifndef _SSMK_SSMK_HPP_
#define _SSMK_SSMK_HPP_

#include <ssmk/context.hpp>
#include <ssmk/sprite.hpp>

#include <functional>

#define CALLBACK(NAME, ...) \
	private: \
		std::function<void(const sm::context& __VA_OPT__(,) __VA_ARGS__)> m_##NAME##_callback; \
	public: \
		void set_##NAME##_callback(const std::function<void(const sm::context& __VA_OPT__(,) __VA_ARGS__)>& callback) { \
			m_##NAME##_callback = callback; \
		} \
		const std::function<void(const sm::context& __VA_OPT__(,) __VA_ARGS__)>& NAME##_callback() const { \
			return m_##NAME##_callback; \
		} \
		std::function<void(const sm::context& __VA_OPT__(,) __VA_ARGS__)>& NAME##_callback() { \
			return m_##NAME##_callback; \
		}

namespace sm {

class ssmk {
public:
	ssmk(const sm::context& context = {}): m_context(context) {};

	sm::context& context = m_context;

public:
	struct CallbackInfo {
		const ssmk& ssmk;
	};

public:
	void read_config();
	CALLBACK(config_read)

	void find_files();
	CALLBACK(
		file_found
	)
	CALLBACK(
		files_found
	)

	void read_sprite_headers();
	CALLBACK(
		image_header_read,
		std::size_t image
	)
	CALLBACK(
		image_headers_read,
	)

	void pack_sprites();
	CALLBACK(
		image_packed,
		std::size_t imageNo
	)
	CALLBACK(
		images_packed
	)

	void make_output_png();

	void build_png_chunk();
	CALLBACK(
		png_chunk_entry_written,
		std::size_t sprite
	)
	CALLBACK(
		png_chunk_built
	)

	void copy_sprites();
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

	void write_png();
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
		read_config();
		find_files();
		read_sprite_headers();
		pack_sprites();
		make_output_png();
		copy_sprites();
		build_png_chunk();
		write_png();
	}

	static void fill_context(sm::context& context);

public:
	constexpr static const std::array configFilenames = {
		"ssmk.toml", "sprite.toml", "spritesheet.toml"
	};
	constexpr static const char* chunk_name = "ssMK";

private:
	sm::context m_context;
};

#undef CALLBACK

}

#endif // !_SSMK_SSMK_HPP_
