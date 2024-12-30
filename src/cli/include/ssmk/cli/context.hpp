#ifndef _SSMK_CLI_CONTEXT_HPP_
#define _SSMK_CLI_CONTEXT_HPP_

#include <filesystem>
#include <type_traits>
#include <ostream>
	
namespace sm::cli {

struct Context {

	enum class Verbosity {
		Silent, 
		Regular, 
		Extra,
		Debug,
	};

	std::filesystem::path sourceDirectory;
	std::underlying_type<Verbosity>::type verbosity =
	static_cast<std::underlying_type<Verbosity>::type>(Verbosity::Regular);

	friend std::ostream& operator<<(std::ostream& os, const Context& c) {
		#define S(PROP) << #PROP ": " << c.PROP << std::endl

		return os
			S(sourceDirectory)
			S(verbosity);

		#undef S
	}
};

}

#endif // !_SSMK_CLI_CONTEXT_HPP_
