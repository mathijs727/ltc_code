#include "ltc/brdf_beckmann.h"
#include <cmath>
#include <glm/geometric.hpp>

namespace ltc {

static float lambda(const float alpha, const float cosTheta)
{
    const float a = 1.0f / alpha / std::tan(std::acos(cosTheta));
    return (cosTheta < 1.0f) ? (1.0f - 1.259f * a + 0.396f * a * a) / (3.535f * a + 2.181f * a * a) : 0.0f;
}

float BrdfBeckmann::eval(const glm::vec3& V, const glm::vec3& L, const float alpha, float& pdf) const
{
    if (V.z <= 0) {
        pdf = 0;
        return 0;
    }

    // masking
    const float LambdaV = lambda(alpha, V.z);

    // shadowing
    float G2;
    if (L.z <= 0.0f)
        G2 = 0;
    else {
        const float LambdaL = lambda(alpha, L.z);
        G2 = 1.0f / (1.0f + LambdaV + LambdaL);
    }

    // D
    const glm::vec3 H = glm::normalize(V + L);
    const float slopex = H.x / H.z;
    const float slopey = H.y / H.z;
    float D = std::exp(-(slopex * slopex + slopey * slopey) / (alpha * alpha)) / (3.14159f * alpha * alpha * H.z * H.z * H.z * H.z);

    pdf = std::abs(D * H.z / 4.0f / glm::dot(V, H));
    float res = D * G2 / 4.0f / V.z;

    return res;
}

glm::vec3 BrdfBeckmann::sample(const glm::vec3& V, const float alpha, const float U1, const float U2) const
{
    const float phi = 2.0f * 3.14159f * U1;
    const float r = alpha * std::sqrt(-logf(U2));
    const glm::vec3 N = glm::normalize(glm::vec3(r * std::cos(phi), r * std::sin(phi), 1.0f));
    const glm::vec3 L = -V + 2.0f * N * glm::dot(N, V);
    return L;
}

}
