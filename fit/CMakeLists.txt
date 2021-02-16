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
	"float_to_half.cpp"
	"float_to_half.h"
	"LTC.cpp"
	"LTC.h"
	"nelder_mead.h"
	"plot.cpp"
	"plot.h"
)
target_link_libraries(LTCFitLib
	PUBLIC glm CImg::CImg
	PRIVATE TBB::tbb)