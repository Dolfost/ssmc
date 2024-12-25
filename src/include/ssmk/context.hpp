#ifndef _SSMK_CONTEXT_HPP_
#define _SSMK_CONTEXT_HPP_

#include <string>
#include <type_traits>

#include <CLI/CLI.hpp>

namespace sm {

class Context {
public:
	Context(std::string name, std::string ver);
	bool parse(int argc, const char** argv);

	enum class Verbosity {
		Silent, 
		Regular, 
		Extra,
		Debug,
	};

public:
	std::underlying_type<Verbosity>::type verbosity;
	std::string sourceDirectory;
	CLI::App app;
};

}

#endif // !_SSMK_CONTEXT_HPP_
