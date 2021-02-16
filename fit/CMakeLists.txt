add_executable(LTCFitLib 
	"brdf.h"
	"brdf_beckmann.cpp"
	"brdf_beckmann.h"
	"brdf_disney_diffuse.cpp"
	"brdf_disney_diffuse.h"
	"brdf_ggx.cpp"
	"brdf_ggx.h"
	"dds.cpp"
	"dds.h"
	"export.cpp"
	"export.h"
	"fitLTC.cpp"
	"float_to_half.h"
	"LTC.h"
	"nelder_mead.h"
	"plot.h"
 "export.cpp" "float_to_half.cpp" "LTC.cpp"  "plot.cpp")
target_link_libraries(LTCFitLib PUBLIC glm CImg::CImg)