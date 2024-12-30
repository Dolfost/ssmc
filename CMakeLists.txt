cmake_minimum_required(VERSION 3.25)

list(APPEND CMAKE_MODULE_PATH 
	"${CMAKE_CURRENT_LIST_DIR}/cmake"
)

include(cmake-scripts/gitversion)
gitversion()

project(ssmk-kit
	LANGUAGES CXX
	VERSION ${GIT_VERSION_TAG}
	DESCRIPTION "Sprite sheet make program kit"
)

add_subdirectory(src)
