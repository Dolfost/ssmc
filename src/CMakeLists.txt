include(FetchContent)

set(CMAKE_CXX_STANDARD 17)

FetchContent_Declare(
	argparse
	GIT_REPOSITORY https://github.com/p-ranav/argparse.git
)
FetchContent_MakeAvailable(argparse)

add_executable(ssmk
	main.cpp
)

target_link_libraries(ssmk
	argparse
)
