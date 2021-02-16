#pragma once
#include "brdf.h"

class BrdfGGX : public Brdf {
public:
    virtual float eval(const glm::vec3& V, const glm::vec3& L, const float alpha, float& pdf) const
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
        float D = 1.0f / (1.0f + (slopex * slopex + slopey * slopey) / alpha / alpha);
        D = D * D;
        D = D / (3.14159f * alpha * alpha * H.z * H.z * H.z * H.z);

        pdf = std::abs(D * H.z / 4.0f / glm::dot(V, H));
        float res = D * G2 / 4.0f / V.z;

        return res;
    }

    virtual glm::vec3 sample(const glm::vec3& V, const float alpha, const float U1, const float U2) const
    {
        const float phi = 2.0f * 3.14159f * U1;
        const float r = alpha * std::sqrt(U2 / (1.0f - U2));
        const glm::vec3 N = glm::normalize(glm::vec3(r * std::cos(phi), r * std::sin(phi), 1.0f));
        const glm::vec3 L = -V + 2.0f * N * glm::dot(N, V);
        return L;
    }

private:
    float lambda(const float alpha, const float cosTheta) const
    {
        const float a = 1.0f / alpha / std::tan(std::acos(cosTheta));
        return (cosTheta < 1.0f) ? 0.5f * (-1.0f + std::sqrt(1.0f + 1.0f / a / a)) : 0.0f;
    }
};
