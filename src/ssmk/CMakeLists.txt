include(FetchContent)
FetchContent_Declare(
    calgo QUIET
    GIT_REPOSITORY https://github.com/Dolfost/calgo.git
    GIT_TAG v3.3.0.0
)
FetchContent_MakeAvailable(calgo)
FetchContent_Declare(
    tomlplusplus QUIET
    GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
    GIT_TAG v3.4.0
)
FetchContent_MakeAvailable(tomlplusplus)

find_package(PNG REQUIRED)

project(ssmk
	LANGUAGES CXX
	VERSION 0.1.1.0
	DESCRIPTION 
		"Sprite sheet make program library"
)

add_library(sm_ssmk 
	src/find_files.cpp
	src/read_sprite_headers.cpp
	src/pack_sprites.cpp
	src/make_output_png.cpp
	src/copy_sprites.cpp
	src/build_png_chunk.cpp
	src/write_png.cpp
	# miscellaneous
	src/sprite.cpp
	src/context.cpp
	src/fill_context.cpp
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
			include/ssmk/ssmk.hpp
			"${CMAKE_CURRENT_BINARY_DIR}/include/ssmk/version.hpp"
			include/ssmk/context.hpp
			include/ssmk/sprite.hpp
			include/ssmk/exceptions.hpp
)

target_link_libraries(sm_ssmk
	tomlplusplus::tomlplusplus 
	PNG::PNG
	ca::optim
)

add_subdirectory(tests)
