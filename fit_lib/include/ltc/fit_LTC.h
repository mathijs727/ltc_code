#pragma once
#include "brdf.h"
#include <glm/fwd.hpp>

namespace ltc {

// Multi threaded and original single threaded version.
void fitTab(glm::mat3* tab, glm::vec2* tabMagFresnel, const int N, const Brdf& brdf);
void fitTabOrig(glm::mat3* tab, glm::vec2* tabMagFresnel, const int N, const Brdf& brdf);

void genSphereTab(float* tabSphere, int N);
void packTab(
    glm::vec4* tex1, glm::vec4* tex2,
    const glm::mat3* tab,
    const glm::vec2* tabMagFresnel,
    const float* tabSphere,
    int N);
}