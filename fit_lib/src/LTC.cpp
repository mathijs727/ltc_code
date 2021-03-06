#include "LTC.h"
#include <cmath>
#include <glm/common.hpp>
#include <glm/geometric.hpp>
#include <iostream>

namespace ltc {

LTC::LTC()
{
    magnitude = 1;
    fresnel = 1;
    m11 = 1;
    m22 = 1;
    m13 = 0;
    X = glm::vec3(1, 0, 0);
    Y = glm::vec3(0, 1, 0);
    Z = glm::vec3(0, 0, 1);
    update();
}

void LTC::update() // compute matrix from parameters
{
    M = glm::mat3(X, Y, Z) * glm::mat3(m11, 0, 0, 0, m22, 0, m13, 0, 1);
    invM = inverse(M);
    detM = abs(glm::determinant(M));
}

float LTC::eval(const glm::vec3& L) const
{
    glm::vec3 Loriginal = glm::normalize(invM * L);
    glm::vec3 L_ = M * Loriginal;

    float l = length(L_);
    float Jacobian = detM / (l * l * l);

    float D = 1.0f / 3.14159f * glm::max<float>(0.0f, Loriginal.z);

    float res = magnitude * D / Jacobian;
    return res;
}

glm::vec3 LTC::sample(const float U1, const float U2) const
{
    const float theta = std::acos(std::sqrt(U1));
    const float phi = 2.0f * 3.14159f * U2;
    const glm::vec3 L = glm::normalize(M * glm::vec3(std::sin(theta) * std::cos(phi), std::sin(theta) * std::sin(phi), std::cos(theta)));
    return L;
}

}
