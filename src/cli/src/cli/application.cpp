#include <ssmk/cli/application.hpp>

#include <string>

namespace sm::cli {

Application::Application(std::string name): a_name(name) {
	setOptions();
}

}
