include(FetchContent)
FetchContent_Declare(
    calgo
    QUIET
    GIT_REPOSITORY https://github.com/Dolfost/calgo.git
    GIT_TAG v3.2.0.0
)
FetchContent_MakeAvailable(calgo)

add_library(sm_ssmk INTERFACE)
add_library(sm::ssmk ALIAS sm_ssmk)

configure_file(include/ssmk/version.hpp.in include/ssmk/version.hpp)

target_sources(sm_ssmk
	PUBLIC FILE_SET HEADERS
		BASE_DIRS 
			"$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>"
			"include"
		FILES 
			"${CMAKE_CURRENT_BINARY_DIR}/include/ssmk/version.hpp"
)
