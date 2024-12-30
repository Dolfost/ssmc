#ifndef _SSMK_CLI_OPTIONS_HPP_
#define _SSMK_CLI_OPTIONS_HPP_

#include <string>

#include <CLI/CLI.hpp>

#include <ssmk/context.hpp>
#include <ssmk/ssmk.hpp>
#include <ssmk/cli/context.hpp>

namespace sm::cli {

class Application {
public:
	Application(std::string name = "");

	const std::string& name() { return a_name; }
	void setName(const std::string& name) { a_name = name; }
	const std::string& version() { return a_version; }
	void setVersion(const std::string& version) { a_version = version; }
	const std::string& description() { return a_description; }
	void setDescription(const std::string& description) { 
		a_description = description; 
	}

	sm::cli::Context& context = a_context;

public:
	int run(int argc, const char** argv);
	int parse(int argc, const char** argv);

private:
	void setOptions();
	void setOptionsStrings();

private:
	sm::cli::Context a_context;
	std::string a_name;
	std::string a_description;
	std::string a_version;

private:
	sm::Ssmk a_ssmk;

private:
	CLI::App a_app;
};

#undef GETSET

}

#endif // !_SSMK_CLI_OPTIONS_HPP_
