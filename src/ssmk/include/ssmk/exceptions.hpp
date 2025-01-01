#ifndef _SSMK_SSMK_EXCEPTIONS_HPP_
#define _SSMK_SSMK_EXCEPTIONS_HPP_

#include <stdexcept>
#include <string>
#include <filesystem>
#include <cstdlib>

namespace sm::ex {

class Error: public std::runtime_error {
public:
	Error(const std::string& what, int code): std::runtime_error(what), e_code(code) {};
	int code() { return e_code; }

private:
	int e_code;
};

namespace code {
enum {
	Good = EXIT_SUCCESS, 
	Bad = EXIT_FAILURE, 
	SsmkErrorSpaceStart = 1400,
	BadSourceDirectoryPath,
	ConfigNotFound,
	ParseError,
	NotFileOrDirectory,
	SsmkErrorSpaceEnd,
};
}

class BadPath: public Error {
public:
	BadPath(
		const std::filesystem::path& path,
		const std::string& what = "Bad path", 
		int code = EXIT_FAILURE
	): Error(what, code) {
		e_path = path;
	};
	const std::filesystem::path& path() {
		return e_path;
	}

private:
	std::filesystem::path e_path;
};

#define PATHERROR(NAME, MSG) \
class NAME: public BadPath { \
public: \
	NAME( \
		const std::filesystem::path& path, \
		const std::string& what = MSG, \
		int code = code::NAME \
	): BadPath(what, path, code) {} \
};

PATHERROR(BadSourceDirectoryPath, "Source directory does not exists")
PATHERROR(ConfigNotFound, "Config file not found")
PATHERROR(NotFileOrDirectory, "Path is not a file or directory")

class ParseError: public Error {
public:
	struct Position {
		Position(std::size_t l = 0, std::size_t c = 0) {
			line = l, column = c;
		}
		std::size_t line;
		std::size_t column;
	};

public:
	ParseError(
		const std::filesystem::path& path,
		const std::string& what = "Parse error", 
		Position begin = {}, Position end = {},
		int code = code::ParseError
	): Error(what, code), 
		e_path(path),
		e_begin(begin), 
		e_end(end) {};
	const std::filesystem::path& path() {
		return e_path;
	}
	const Position& begin() { return e_begin; }
	const Position& end() { return e_end; }

private:
	Position e_begin, e_end;
	std::filesystem::path e_path;
};

#define PARSEERROR(NAME, MSG) \
class NAME: public ParseError { \
public: \
	NAME( \
		const std::filesystem::path& path, \
		Position begin = {}, Position end = {}, \
		const std::string& what = #MSG,  \
		int code = code::ParseError \
	): ParseError(path, what, begin, end, code) {} \
}

PARSEERROR(NoInputTable, "Input table not defined");
PARSEERROR(NoFileArray, "No input file array defined");
PARSEERROR(NotHomogeneousArray, "Array is not homogeneous");
PARSEERROR(NoOutputTable, "Output table is not defined");
PARSEERROR(NoOutputFile, "Output file is not defined");

#undef PATHERROR
#undef PARSEERROR

}


#endif // !_SSMK_SSMK_EXCEPTIONS_HPP_
