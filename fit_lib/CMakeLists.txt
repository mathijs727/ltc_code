add_library(ltc 
	"src/brdf_beckmann.cpp"
	"src/brdf_disney_diffuse.cpp"
	"src/brdf_ggx.cpp"
	"src/dds.cpp"
	"src/export.cpp"
	"src/fit_LTC.cpp"
	"src/float_to_half.cpp"
	"src/LTC.cpp"
	"src/plot.cpp"
)
target_include_directories(
    ltc
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/src
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)
target_link_libraries(ltc
	PUBLIC glm::glm CImg::CImg
	PRIVATE TBB::tbb fmt::fmt)
target_compile_features(ltc PUBLIC cxx_std_20)