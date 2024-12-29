include(FetchContent)
FetchContent_Declare(
    cli11_proj
    QUIET
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
    GIT_TAG v2.3.2
)
FetchContent_MakeAvailable(cli11_proj)

add_executable(sm_ssmk_cli
	src/main.cpp
	src/options.cpp
)
add_executable(sm::ssmk::cli ALIAS sm_ssmk_cli)

set_target_properties(sm_ssmk_cli PROPERTIES
	VERSION ${PROJECT_VERSION}
	OUTPUT_NAME ssmk
	EXPORT_NAME ssmkcli
)

target_sources(sm_ssmk_cli
	PUBLIC FILE_SET HEADERS
		BASE_DIRS 
			"include"
		FILES 
			include/ssmk/cli/context.hpp
)

target_link_libraries(sm_ssmk_cli
	CLI11::CLI11 sm::ssmk
)
