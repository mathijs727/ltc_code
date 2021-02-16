add_executable(LTCFitLib 
	"brdf.h"
	"brdf_beckmann.h"
	"brdf_disneyDiffuse.h"
	"brdf_ggx.h"
	"dds.cpp"
	"dds.h"
	"export.h"
	"fitLTC.cpp"
	"float_to_half.h"
	"LTC.h"
	"nelder_mead.h"
	"plot.h"
)
target_link_libraries(LTCFitLib PUBLIC glm CImg::CImg)