#ifndef _SSMK_CLI_CONTEXT_HPP_
#define _SSMK_CLI_CONTEXT_HPP_

#include <string>
#include <type_traits>
	
namespace sm::cli {

struct Context {

	enum class Verbosity {
		Silent, 
		Regular, 
		Extra,
		Debug,
	};

	std::underlying_type<Verbosity>::type verbosity =
	static_cast<std::underlying_type<Verbosity>::type>(Verbosity::Regular);
};

}

#endif // !_SSMK_CLI_CONTEXT_HPP_
