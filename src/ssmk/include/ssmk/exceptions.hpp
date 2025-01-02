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
enum Type {
	Good = EXIT_SUCCESS, 
	Bad = EXIT_FAILURE, // Error class

	SsmkErrorSpaceStart = 1400,

	NoSpritesFound,

	FileError, // class from Error
	ConfigNotFound,
	SourceDirectoryNotFound,
	NotAFileOrDirectory,

	PngError, // class from FileError
	PngFailedToOpenForReading,
	PngFailedToOpenForWritting,
	PngBadSignature,
	PngCouldNotCreateReadStructure,
	PngCouldNotCreateInfoStructure,

	TomlError, // class from FileError

	ConfigError, // class from FileError
	ConfigFieldError, // class from FileError
	ConfigNoInputTable,
	ConfigNoOutputTable,
	ConfigNoInputFileArray,
	ConfigNoOutputFile,
	ConfigWrongFieldType, // class from FieldError

	SsmkErrorSpaceEnd,
};
static const std::unordered_map<Type, const std::string> text = {
	{ Good, "" },
	{ Bad, "SSMK error"},

	{ SsmkErrorSpaceStart, "SSMK error space start" },

	{ NoSpritesFound, "no sprites found" },

	{ FileError, "file error" },
	{ ConfigNotFound, "config not found" },
	{ SourceDirectoryNotFound, "source directory not found" },
	{ NotAFileOrDirectory, "not a file or directory" },

	{ PngError, "png error" },
	{ PngFailedToOpenForReading, "failed to open image for reading" },
	{ PngFailedToOpenForWritting, "failed to open image for writting" },
	{ PngBadSignature, "image signature does not match a png" },
	{ PngCouldNotCreateReadStructure, "could not create read structure" },
	{ PngCouldNotCreateInfoStructure, "could not create info structure" },

	{ TomlError, "toml parse failed" },

	{ ConfigError, "config error" },
	{ ConfigFieldError, "filed error" },
	{ ConfigNoInputTable, "input table not defined" },
	{ ConfigNoOutputTable, "output table not defined" },
	{ ConfigNoInputFileArray, "input file array not defined" },
	{ ConfigNoOutputFile, "output file not specified" },
	{ ConfigWrongFieldType, "field type" },

	{ SsmkErrorSpaceEnd, "SSMK error space end" },
};
}

class Error: public std::runtime_error {
public:
	Error(
		const std::string& description, 
		code::Type code = code::Bad,
		const std::string& what = code::text.at(code::Bad)
	): std::runtime_error(what), e_code(code), e_description(description) {};
	code::Type code() { return e_code; }
	const std::string& description() { return e_description; };

private:
	code::Type e_code;
	std::string e_description;
};

class FileError: public Error {
public:
	FileError(
		const std::filesystem::path& path,
		const std::string& description = code::text.at(code::FileError),
		code::Type code = code::FileError,
		const std::string& what = code::text.at(code::FileError)
	): Error(description, code, what), e_path(path) {};
	const std::filesystem::path& path() {
		return e_path;
	}
private:
	std::filesystem::path e_path;
};

class PngError: public FileError {
public:
	PngError(
		const std::filesystem::path& path,
		const std::string& description = code::text.at(code::PngError),
		code::Type code = code::FileError,
		const std::string& what = code::text.at(code::PngError)
	): FileError(path, description, code, what) {};
};

class TomlError: public FileError {
public:
	struct Position {
		Position(std::size_t l = 0, std::size_t c = 0) {
			line = l, column = c;
		}
		std::size_t line;
		std::size_t column;
	};

public:
	TomlError(
		const std::filesystem::path& path,
		Position begin, Position end,
		const std::string& description,
		code::Type code = code::TomlError,
		const std::string& what = code::text.at(code::TomlError)
	): FileError(path, description, code, what), 
		e_begin(begin), 
		e_end(end) {};
	const Position& begin() { return e_begin; }
	const Position& end() { return e_end; }

private:
	Position e_begin, e_end;
};

class ConfigError: public FileError {
public:
	ConfigError(
		const std::filesystem::path& path,
		const std::string& description = code::text.at(code::ConfigError),
		code::Type code = code::ConfigError,
		const std::string& what = code::text.at(code::ConfigError)
	): FileError(path, description, code, what) {};
};

class ConfigFieldError: public ConfigError {
public:
	ConfigFieldError(
		const std::filesystem::path& path,
		const std::string& field,
		const std::string& description = code::text.at(code::ConfigFieldError),
		code::Type code = code::ConfigFieldError,
		const std::string& what = code::text.at(code::ConfigFieldError)
	): ConfigError(path, description, code, what), e_field(field) {};
	const std::string& field() { return e_field; };
private:
	std::string e_field;
};

class ConfigWrongFieldType: public ConfigFieldError {
public:
	ConfigWrongFieldType(
		const std::filesystem::path& path,
		const std::string& field,
		const std::string& type,
		const std::string& expectedType,
		const std::string& description = code::text.at(code::ConfigWrongFieldType),
		code::Type code = code::ConfigFieldError,
		const std::string& what = code::text.at(code::ConfigFieldError)
	): ConfigFieldError(path, description, field, code, what), 
		e_type(type), e_expectedType(expectedType) {};
	std::string& type() { return e_type; }
	std::string& expectedType() { return e_expectedType; }
private:
	std::string e_expectedType;
	std::string e_type;
};

}


#endif // !_SSMK_SSMK_EXCEPTIONS_HPP_
