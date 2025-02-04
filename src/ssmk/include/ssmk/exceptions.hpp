#ifndef _SSMK_SSMK_EXCEPTIONS_HPP_
#define _SSMK_SSMK_EXCEPTIONS_HPP_

#include <stdexcept>
#include <string>
#include <filesystem>
#include <cstdlib>
#include <unordered_map>

#define SM_EX_THROW(EX, CODE, ...) \
throw sm::ex::EX( \
	__VA_ARGS__ __VA_OPT__(,) \
	sm::ex::code::text.at(sm::ex::code::CODE), sm::ex::code::CODE \
);

namespace sm::ex {

namespace code {
enum type {
	good = EXIT_SUCCESS, 
	bad = EXIT_FAILURE, // error class

	ssmk_error_space_start = 1400,

	no_sprites_found,
	no_packing_algorithm,
	no_packing_metric,
	not_decreasing_tree_fit_packing,

	file_error, // class from error
	config_not_found,
	source_directory_not_found,
	not_a_file_or_directory,

	png_error, // class from file_error
	png_failed_to_open_for_reading,
	png_failed_to_open_for_writting,
	png_bad_signature,
	png_could_not_create_read_structure,
	png_could_not_create_write_structure,
	png_could_not_create_info_structure,
	png_could_not_allocate_output_rows,
	png_could_not_allocate_background_color,
	png_no_ssmk_chunk,

	toml_error, // class from file_error

	config_error, // class from file_error
	config_field_error, // class from file_error
	config_no_input_table,
	config_no_output_table,
	config_no_input_file_array,
	config_no_output_file,
	config_wrong_field_type, // class from field_error
	config_unexpected_field_value, // class from field_error
	config_unknown_packing_algorithm,
	config_unknown_packing_metric,
	config_not_RGB,
	config_unknown_compression_level,
	config_exclusive_field_values, // class from field_error
	config_increasing_tree_fit_packing,
	
	ssmk_error_space_end,
};
static const std::unordered_map<type, const std::string> text = {
	{ good, "" },
	{ bad, "SSMK error"},

	{ ssmk_error_space_start, "SSMK error space start" },

	{ no_sprites_found, "no sprites found" },

	{ no_packing_algorithm, "no packing algorithm" },
	{ no_packing_metric, "no packing metric" },
	{ not_decreasing_tree_fit_packing, "increasing tree fit packing" },

	{ file_error, "file error" },
	{ config_not_found, "config not found" },
	{ source_directory_not_found, "source directory not found" },
	{ not_a_file_or_directory, "not a file or directory" },

	{ png_error, "png error" },
	{ png_failed_to_open_for_reading, "failed to open image for reading" },
	{ png_failed_to_open_for_writting, "failed to open image for writting" },
	{ png_bad_signature, "image signature does not match a png" },
	{ png_could_not_create_read_structure, "could not create read structure" },
	{ png_could_not_create_write_structure, "could not create write structure" },
	{ png_could_not_create_info_structure, "could not create info structure" },
	{ png_could_not_allocate_output_rows, "could not allocate memory for output image" },
	{ png_could_not_allocate_background_color, "could not allocate background color" },
	{ png_no_ssmk_chunk, "ssmk chunk not found" },

	{ toml_error, "toml parse failed" },

	{ config_error, "config error" },
	{ config_field_error, "field error" },
	{ config_no_input_table, "input table not defined" },
	{ config_no_output_table, "output table not defined" },
	{ config_no_input_file_array, "input file array not defined" },
	{ config_no_output_file, "output file not specified" },
	{ config_wrong_field_type, "unexpected type" },
	{ config_unexpected_field_value, "unexpected value" },
	{ config_unknown_packing_algorithm, "unknown packing algorithm" },
	{ config_unknown_packing_metric, "unknown packing metric" },
	{ config_not_RGB, "invalid RGB value" },
	{ config_exclusive_field_values, "exclusive values" },
	{ config_increasing_tree_fit_packing, "increasing first fit packing is forbidden" },
	{ config_unknown_compression_level, "unknown compression level" },

	{ ssmk_error_space_end, "SSMK error space end" },
};
}

class error: public std::runtime_error {
public:
	error(
		const std::string& description, 
		code::type code = code::bad,
		const std::string& what = code::text.at(code::bad)
	): std::runtime_error(what), m_code(code), m_description(description) {};
	code::type code() { return m_code; }
	const std::string& description() { return m_description; };

private:
	code::type m_code;
	std::string m_description;
};

class file_error: public error {
public:
	file_error(
		const std::filesystem::path& path,
		const std::string& description = code::text.at(code::file_error),
		code::type code = code::file_error,
		const std::string& what = code::text.at(code::file_error)
	): error(description, code, what), m_path(path) {};
	const std::filesystem::path& path() {
		return m_path;
	}
private:
	std::filesystem::path m_path;
};

class png_error: public file_error {
public:
	png_error(
		const std::filesystem::path& path,
		const std::string& description = code::text.at(code::png_error),
		code::type code = code::file_error,
		const std::string& what = code::text.at(code::png_error)
	): file_error(path, description, code, what) {};
};

class toml_error: public file_error {
public:
	struct position {
		position(std::size_t l = 0, std::size_t c = 0) {
			line = l, column = c;
		}
		std::size_t line;
		std::size_t column;
	};

public:
	toml_error(
		const std::filesystem::path& path,
		position begin, position end,
		const std::string& description,
		code::type code = code::toml_error,
		const std::string& what = code::text.at(code::toml_error)
	): file_error(path, description, code, what), 
		m_begin(begin), 
		m_end(end) {};
	const position& begin() { return m_begin; }
	const position& end() { return m_end; }

private:
	position m_begin, m_end;
};

class config_error: public file_error {
public:
	config_error(
		const std::filesystem::path& path,
		const std::string& description = code::text.at(code::config_error),
		code::type code = code::config_error,
		const std::string& what = code::text.at(code::config_error)
	): file_error(path, description, code, what) {};
};

class config_field_error: public config_error {
public:
	config_field_error(
		const std::filesystem::path& path,
		const std::string& field,
		const std::string& description = code::text.at(code::config_field_error),
		code::type code = code::config_field_error,
		const std::string& what = code::text.at(code::config_field_error)
	): config_error(path, description, code, what), m_field(field) {};
	const std::string& field() { return m_field; };
private:
	std::string m_field;
};

class config_wrong_field_type: public config_field_error {
public:
	config_wrong_field_type(
		const std::filesystem::path& path,
		const std::string& field,
		const std::string& type,
		const std::string& expected_type,
		const std::string& description = code::text.at(code::config_wrong_field_type),
		code::type code = code::config_field_error,
		const std::string& what = code::text.at(code::config_field_error)
	): config_field_error(path, field, description, code, what), 
		m_type(type), m_expectedType(expected_type) {};
	const std::string& type() { return m_type; }
	const std::string& expectedType() { return m_expectedType; }
private:
	std::string m_expectedType;
	std::string m_type;
};

class config_unexpected_field_value: public config_field_error {
public:
	template<class T>
	config_unexpected_field_value(
		const std::filesystem::path& path,
		const std::string& field,
		const std::string& value,
		const T& expected,
		const std::string& description = code::text.at(code::config_unexpected_field_value),
		code::type code = code::config_field_error,
		const std::string& what = code::text.at(code::config_field_error)
	): config_field_error(path, field, description, code, what), 
		m_value(value) {
		for (const auto& [k, v]: expected) {
			m_expected += k + ", ";
		}
		m_expected.erase(m_expected.size()-2, 2);
	};
	config_unexpected_field_value(
		const std::filesystem::path& path,
		const std::string& field,
		const std::string& value,
		const std::string& expected,
		const std::string& description = code::text.at(code::config_unexpected_field_value),
		code::type code = code::config_field_error,
		const std::string& what = code::text.at(code::config_field_error)
	): config_field_error(path, field, description, code, what), 
		m_value(value), m_expected(expected) {};
	const std::string& value() { return m_value; }
	const std::string& expected() { return m_expected; }
private:
	std::string m_value;
	std::string m_expected;
};

class config_exclusive_field_values: public config_field_error {
public:
	config_exclusive_field_values(
		const std::filesystem::path& path,
		const std::string& field1,
		const std::string& value1,
		const std::string& field2,
		const std::string& value2,
		const std::string& description = code::text.at(code::config_exclusive_field_values),
		code::type code = code::config_field_error,
		const std::string& what = code::text.at(code::config_field_error)
	): config_field_error(path, description, field1, code, what), 
		m_value1(value1) {};
	const std::string& value1() { return m_value1; }
	const std::string& value2() { return m_value2; }
	const std::string& field1() { return field(); }
	const std::string& field2() { return m_field2; }
private:
	std::string m_value1, m_value2, m_field2;
};

}

#endif // !_SSMK_SSMK_EXCEPTIONS_HPP_
