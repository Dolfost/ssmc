set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(
    cli11_proj
    QUIET
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
    GIT_TAG v2.3.2
)
FetchContent_MakeAvailable(cli11_proj)

add_executable(sm_ssmk
	main.cpp
	context.cpp
)
add_executable(sm::ssmk ALIAS sm_ssmk)

set_target_properties(sm_ssmk PROPERTIES
	VERSION ${PROJECT_VERSION}
	OUTPUT_NAME ssmk
	EXPORT_NAME ssmk
)

configure_file(include/ssmk/version.hpp.in include/ssmk/version.hpp)
target_sources(sm_ssmk
	PUBLIC FILE_SET HEADERS
		BASE_DIRS 
			"$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>"
			"include"
		FILES 
			include/ssmk/context.hpp
			"${CMAKE_CURRENT_BINARY_DIR}/include/ssmk/version.hpp"
)

target_link_libraries(sm_ssmk
	CLI11::CLI11
)
