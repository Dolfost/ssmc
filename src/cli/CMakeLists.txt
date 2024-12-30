include(FetchContent)
FetchContent_Declare(
    cli11_proj QUIET
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
    GIT_TAG v2.3.2
)
FetchContent_MakeAvailable(cli11_proj)

project(ssmk-cli
	LANGUAGES CXX
	VERSION 0.1.1.0
	DESCRIPTION 
		"Sprite sheet make program command line interface"
)

add_executable(sm_ssmk_cli
	src/main.cpp
	src/cli/application.cpp
	src/cli/options.cpp
	src/cli/run.cpp
)
add_executable(sm::ssmk::cli ALIAS sm_ssmk_cli)

configure_file(
	include/ssmk/cli/version.hpp.in 
	include/ssmk/cli/version.hpp
)

set_target_properties(sm_ssmk_cli PROPERTIES
	VERSION ${PROJECT_VERSION}
	OUTPUT_NAME ssmk
	EXPORT_NAME ssmkcli
)

target_sources(sm_ssmk_cli
	PUBLIC FILE_SET HEADERS
		BASE_DIRS 
			"$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>"
			"include"
		FILES 
			"${CMAKE_CURRENT_BINARY_DIR}/include/ssmk/cli/version.hpp"
			include/ssmk/cli/application.hpp
			include/ssmk/cli/context.hpp
)

target_link_libraries(sm_ssmk_cli
	CLI11::CLI11 sm::ssmk
)
