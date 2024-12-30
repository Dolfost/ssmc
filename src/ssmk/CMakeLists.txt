include(FetchContent)
FetchContent_Declare(
    calgo
    QUIET
    GIT_REPOSITORY https://github.com/Dolfost/calgo.git
    GIT_TAG v3.2.0.0
)
FetchContent_MakeAvailable(calgo)

project(ssmk
	LANGUAGES CXX
	VERSION 0.1.1.0
	DESCRIPTION 
		"Sprite sheet make program library"
)

add_library(sm_ssmk 
	src/readConfig.cpp
)
add_library(sm::ssmk ALIAS sm_ssmk)

configure_file(
	include/ssmk/version.hpp.in 
	include/ssmk/version.hpp
)

target_sources(sm_ssmk
	PUBLIC FILE_SET HEADERS
		BASE_DIRS 
			"$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>"
			"include"
		FILES 
			"${CMAKE_CURRENT_BINARY_DIR}/include/ssmk/version.hpp"
			include/ssmk/context.hpp
			include/ssmk/ssmk.hpp
			include/ssmk/exceptions.hpp
)
