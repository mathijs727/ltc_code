#include "ltc/brdf_disney_diffuse.h"
#include <cmath>
#include <glm/geometric.hpp>

namespace ltc {

float BrdfDisneyDiffuse::eval(const glm::vec3& V, const glm::vec3& L, const float alpha, float& pdf) const
{
    if (V.z <= 0 || L.z <= 0) {
        pdf = 0;
        return 0;
    }

    pdf = L.z / 3.14159f;

    float NdotV = V.z;
    float NdotL = L.z;
    float LdotH = glm::dot(L, glm::normalize(V + L));
    float perceptualRoughness = std::sqrt(alpha);
    float fd90 = 0.5f + 2 * LdotH * LdotH * perceptualRoughness;
    float lightScatter = (1 + (fd90 - 1) * std::pow(1 - NdotL, 5.0f));
    float viewScatter = (1 + (fd90 - 1) * std::pow(1 - NdotV, 5.0f));
    return lightScatter * viewScatter * L.z / 3.14159f;
}

glm::vec3 BrdfDisneyDiffuse::sample(const glm::vec3& V, const float alpha, const float U1, const float U2) const
{
    const float r = std::sqrt(U1);
    const float phi = 2.0f * 3.14159f * U2;
    const glm::vec3 L = glm::vec3(r * std::cos(phi), r * std::sin(phi), std::sqrt(1.0f - r * r));
    return L;
}

}
