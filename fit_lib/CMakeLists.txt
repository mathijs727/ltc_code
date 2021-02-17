add_library(LTCFitLib 
	"include/ltc/brdf.h"
	"include/ltc/brdf_beckmann.h"
	"include/ltc/brdf_disney_diffuse.h"
	"include/ltc/brdf_ggx.h"
	"src/dds.h"
	"include/ltc/export.h"
	"src/float_to_half.h"
	"src/LTC.h"
	"src/nelder_mead.h"
	"include/ltc/plot.h"

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
target_include_directories(LTCFitLib PUBLIC "include" PRIVATE "src")
target_link_libraries(LTCFitLib
	PUBLIC glm CImg::CImg
	PRIVATE TBB::tbb fmt::fmt)
target_compile_features(LTCFitLib PUBLIC cxx_std_20)
