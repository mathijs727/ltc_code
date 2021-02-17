project(LinearlyTransformedCosines)
cmake_minimum_required(VERSION 3.0)

# Installed with vcpkg
find_package(CImg CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(TBB CONFIG REQUIRED)

add_subdirectory("fit_lib")

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
	# Only when this file is not included as a subdirectory in another project.
	add_subdirectory("fit_app")
endif()
