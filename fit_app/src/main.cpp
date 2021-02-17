#include "ltc/brdf_beckmann.h"
#include "ltc/brdf_disney_diffuse.h"
#include "ltc/brdf_ggx.h"
#include "ltc/export.h"
#include "ltc/fit_LTC.h"
#include "ltc/plot.h"
#include <glm/mat3x3.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <vector>

#define PARALLEL 1

// size of precomputed table (theta, alpha)
constexpr int N = 64;

void createFolderIfNotExists(const std::filesystem::path& folderPath) {
    if (!std::filesystem::exists(folderPath))
        std::filesystem::create_directories(folderPath);
}

int main(int argc, char* argv[])
{
    using namespace ltc;

    // BRDF to fit
    BrdfGGX brdf;
    //BrdfBeckmann brdf;
    //BrdfDisneyDiffuse brdf;

    // allocate data
    std::vector<glm::mat3> tab(N * N);
    std::vector<glm::vec2> tabMagFresnel(N * N);
    std::vector<float> tabSphere(N * N);

// fit
#if PARALLEL
    fitTab(tab.data(), tabMagFresnel.data(), N, brdf);
#else
    fitTabOrig(tab.data(), tabMagFresnel.data(), N, brdf);
#endif

    // projected solid angle of a spherical cap, clipped to the horizon
    genSphereTab(tabSphere.data(), N);

    // pack tables (texture representation)
    std::vector<glm::vec4> tex1(N * N);
    std::vector<glm::vec4> tex2(N * N);
    packTab(tex1.data(), tex2.data(), tab.data(), tabMagFresnel.data(), tabSphere.data(), N);

    // export to C, MATLAB and DDS
    writeTabMatlab(tab.data(), tabMagFresnel.data(), N);
    writeTabC(tab.data(), tabMagFresnel.data(), N);
    writeDDS(tex1.data(), tex2.data(), N);
    writeJS(tex1.data(), tex2.data(), N);

    // spherical plots
    createFolderIfNotExists("plots");
    make_spherical_plots(brdf, tab.data(), N, "plots");

    return 0;
}
