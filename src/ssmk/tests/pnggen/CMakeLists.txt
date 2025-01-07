find_package(PNG REQUIRED)

project(pnggen
	LANGUAGES CXX
	DESCRIPTION 
		"Png files generator"
)

add_executable(pnggen
	main.cpp
)

find_package(PNG REQUIRED)
target_link_libraries(pnggen 
	PRIVATE PNG::PNG
)
