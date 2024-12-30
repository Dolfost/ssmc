#ifndef _SSMK_SSMK_EXCEPTIONS_HPP_
#define _SSMK_SSMK_EXCEPTIONS_HPP_

#include <stdexcept>
#include <string>
#include <filesystem>
#include <cstdlib>

namespace sm::ex {

enum class Code;

class Error: public std::runtime_error {
public:
	Error(const std::string& what, Code code): std::runtime_error(what), e_code(code) {};
	Code code() { return e_code; }

private:
	Code e_code;
};

class BadPath: public Error {
public:
	BadPath(
		const std::string& what = "Bad path", 
		const std::filesystem::path& path = "",
		Code code = static_cast<Code>(EXIT_FAILURE)
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
		const std::string& what = MSG, \
		const std::filesystem::path& path = "", \
		Code code = Code::NAME \
	): BadPath(what, path, code) {} \
};

enum class Code {
	Good = EXIT_SUCCESS, 
	Bad = EXIT_FAILURE, 
	SsmkErrorSpaceStart = 1400,
	BadSourceDirectoryPath,
	ConfigNotFound,
};

PATHERROR(BadSourceDirectoryPath, "Source directory does not exists")
PATHERROR(ConfigNotFound, "Config file not found")

#undef PATHERROR

}


#endif // !_SSMK_SSMK_EXCEPTIONS_HPP_
