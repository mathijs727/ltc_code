#pragma once
#include "brdf.h"
#include <filesystem>
#include <glm/fwd.hpp>

namespace ltc {

// raytrace a sphere
// evaluate the BRDF or the LTC
// call the color map
void make_spherical_plots(
    const Brdf& brdf, const glm::mat3* tab, const int N,
    const std::filesystem::path& outFolder);

}
