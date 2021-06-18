cmake_minimum_required(VERSION 3.8)
project(ltc)

option(LTC_BUILD_APP "Build the executable that performs the fitting for the bundled BRDFs" ON)

# Installed with vcpkg
find_package(CImg CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(TBB CONFIG REQUIRED)

add_subdirectory("fit_lib")

if(LTC_BUILD_APP)
	# Only when this file is not included as a subdirectory in another project.
	add_subdirectory("fit_app")
endif()

install(FILES
    "fit_lib/include/ltc/brdf.h"
    "fit_lib/include/ltc/brdf_beckmann.h"
    "fit_lib/include/ltc/brdf_disney_diffuse.h"
    "fit_lib/include/ltc/brdf_ggx.h"
    "fit_lib/include/ltc/export.h"
    "fit_lib/include/ltc/fit_LTC.h"
    "fit_lib/include/ltc/plot.h"
    DESTINATION "include/ltc/"
)
install(
    TARGETS ltc
    EXPORT ${PROJECT_NAME}_target
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

include(CMakePackageConfigHelpers)
configure_package_config_file(ltc-config.cmake.in ltc-config.cmake INSTALL_DESTINATION share/ltc)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ltc-config.cmake DESTINATION share/ltc)

install(
    EXPORT ${PROJECT_NAME}_target
    NAMESPACE "ltc::"
    FILE ${PROJECT_NAME}-targets.cmake
    DESTINATION cmake/
) 