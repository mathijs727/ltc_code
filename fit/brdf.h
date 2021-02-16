#pragma once
#include <glm/glm.hpp>

namespace ltc {

class Brdf {
public:
    // evaluation of the cosine-weighted BRDF
    // pdf is set to the PDF of sampling L
    virtual float eval(const glm::vec3& V, const glm::vec3& L, const float alpha, float& pdf) const = 0;

    // sampling
    virtual glm::vec3 sample(const glm::vec3& V, const float alpha, const float U1, const float U2) const = 0;
};

}
