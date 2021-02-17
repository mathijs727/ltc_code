#pragma once
#include <glm/mat3x3.hpp>
#include <glm/vec3.hpp>

namespace ltc {

struct LTC {
public:
    // lobe magnitude
    float magnitude;

    // Average Schlick Fresnel term
    float fresnel;

    // parametric representation
    float m11, m22, m13;
    glm::vec3 X, Y, Z;

    // matrix representation
    glm::mat3 M;
    glm::mat3 invM;
    float detM;

public:
    LTC();
    void update(); // compute matrix from parameters
    float eval(const glm::vec3& L) const;
    glm::vec3 sample(const float U1, const float U2) const;
};

}
