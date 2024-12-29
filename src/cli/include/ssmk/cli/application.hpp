#ifndef _SSMK_CLI_OPTIONS_HPP_
#define _SSMK_CLI_OPTIONS_HPP_

#include <string>

#include <CLI/CLI.hpp>

#include <ssmk/context.hpp>
#include <ssmk/cli/context.hpp>

namespace sm::cli {

#define GETSET(T, PROP) \
	private: \
		T a_##PROP; \
	public: \
		const T& PROP() const { \
			return a_##PROP; \
		} \
		void PROP(const T& newVal) { \
			a_##PROP = newVal; \
		} \
	private:

class Application {
public:
	Application(std::string name = "");
	bool parse(int argc, const char** argv);

	GETSET(Context, cliContext)
	GETSET(sm::Context, context)
	GETSET(std::string, name)
	GETSET(std::string, description)
	GETSET(std::string, version)

private:
	void setOptions();

private:
	CLI::App a_app;
};

#undef GETSET

}

#endif // !_SSMK_CLI_OPTIONS_HPP_
