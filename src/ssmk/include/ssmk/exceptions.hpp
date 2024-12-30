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

namespace code {
	enum {
		Good = EXIT_SUCCESS, 
		Bad = EXIT_FAILURE, 
		SsmkErrorSpaceStart = 1400,
		BadSourceDirectoryPath,
		ConfigNotFound,
	};
}

PATHERROR(BadSourceDirectoryPath, "Source directory does not exists")
PATHERROR(ConfigNotFound, "Config file not found")

#undef PATHERROR

}


#endif // !_SSMK_SSMK_EXCEPTIONS_HPP_
