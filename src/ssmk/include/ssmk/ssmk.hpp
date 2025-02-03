#ifndef _SSMK_SSMK_HPP_
#define _SSMK_SSMK_HPP_

#include <ssmk/context.hpp>

#include <functional>
#include <cstddef>
#include <type_traits>

#define CALLBACK(NAME, ...) \
	private: \
		std::function<void(const context_type& __VA_OPT__(,) __VA_ARGS__)> m_##NAME##_callback; \
	public: \
		void set_##NAME##_callback(const std::function<void(const context_type& __VA_OPT__(,) __VA_ARGS__)>& callback) { \
			m_##NAME##_callback = callback; \
		} \
		const std::function<void(const context_type& __VA_OPT__(,) __VA_ARGS__)>& NAME##_callback() const { \
			return m_##NAME##_callback; \
		} \
		std::function<void(const context_type& __VA_OPT__(,) __VA_ARGS__)>& NAME##_callback() { \
			return m_##NAME##_callback; \
		}

namespace sm {

class ssmk {
public:
	using size_type = std::size_t;
	using context_type = sm::context;

public:
	ssmk(const context_type& context = {}): m_context(context) {};

	context_type& context = m_context;

public:
	template<typename P> typename 
	std::enable_if<std::is_assignable<std::filesystem::path, P>::value>::type read_config(P&& dir) {
		context.conf.directory = std::forward<P>(dir);
		ssmk::fill_context(context);

		if (m_config_read_callback)
			m_config_read_callback(m_context);
	}

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
		size_type image
	)
	CALLBACK(
		image_headers_read,
	)

	void pack_sprites();
	CALLBACK(
		image_packed,
		size_type imageNo
	)
	CALLBACK(
		images_packed
	)

	void make_output_png();

	void build_png_chunk();
	CALLBACK(
		png_chunk_entry_written,
		size_type sprite
	)
	CALLBACK(
		png_chunk_built
	)

	void copy_sprites();
	CALLBACK(
		sprite_row_copied,
		size_type sprite,
		size_type row,
		size_type pass,
		size_type passes
	)
	CALLBACK(
		sprite_copied,
		size_type image
	)
	CALLBACK(
		sprites_copied
	)

	void write_png();
	CALLBACK(
		image_row_written,
		size_type row,
		size_type pass,
		size_type passes
	)
	CALLBACK(
		png_written
	)

public:
	template<typename P> typename 
	std::enable_if<std::is_assignable<std::filesystem::path, P>::value>::type make_sheet(P&& dir) {
		read_config(std::forward<P>(dir));
		find_files();
		read_sprite_headers();
		pack_sprites();
		make_output_png();
		copy_sprites();
		build_png_chunk();
		write_png();
	}

	void read_sheet_header();
	CALLBACK(
		sheet_header_read
	);

	void extract_png_chunk();
	CALLBACK(
		chunk_entry_extracted,
		size_type i
	);
	CALLBACK(
		chunk_extracted,
	);

	void read_sheet_png_data();
	CALLBACK(
		image_row_read,
		size_type row,
		size_type pass,
		size_type passes
	)
	CALLBACK(
		image_read
	)

	/*
	 * will use m_context.conf.file or find config file in
	 * m_context.conf.directory and extract from it the output.file field and 
	 * read the png and build the std::map<std::string, sprite>
	*/
	template<typename P> typename 
	std::enable_if<std::is_assignable<std::filesystem::path, P>::value>::type read_sheet(P&& path) {
		m_context = sm::context();
		if (std::filesystem::is_regular_file(path))
			m_context.out.file = std::filesystem::absolute(path);
		else 
			read_config(std::forward<P>(path)); // will throw at any file error
		read_sheet_header();
		extract_png_chunk();
		read_sheet_png_data();
	}

	/*
	 * will write read sheet tree to specified directory
	*/
	void extract_sheet() {
	}

	static void fill_context(sm::context& context);

public:
	constexpr static const std::array config_filenames = {
		"ssmk.toml", "sprite.toml", "spritesheet.toml"
	};
	constexpr static const char* chunk_name = "ssMK";

private:
	context_type m_context;
};

#undef CALLBACK

}

#endif // !_SSMK_SSMK_HPP_
