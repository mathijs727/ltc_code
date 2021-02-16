project(LinearlyTransformedCosines)
cmake_minimum_required(VERSION 3.0)

# Installed with vcpkg
find_package(CImg CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(TBB CONFIG REQUIRED)

add_subdirectory("fit")
