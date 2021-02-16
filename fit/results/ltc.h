struct mat33
{
    operator glm::glm::mat3() const
    {
        return glm::glm::mat3(m[0], m[1], m[2], m[3], m[4], m[5], m[6], m[7], m[8]);
    }

    double m[9];
};


#include "ltc.inc"

#if 1
glm::mat3 M_GGX(const float cosTheta, const float alpha)
{
    int t = std::max<int>(0, std::min<int>(size - 1, (int)floorf(std::sqrt(1.0f - cosTheta) * size)));
    int a = std::max<int>(0, std::min<int>(size - 1, (int)floorf(std::sqrt(alpha) * size)));

    return tabM[a + t*size];
}

float magnitude_GGX(const float cosTheta, const float alpha)
{
    int t = std::max<int>(0, std::min<int>(size - 1, (int)floorf(std::sqrt(1.0f - cosTheta) * size)));
    int a = std::max<int>(0, std::min<int>(size - 1, (int)floorf(std::sqrt(alpha) * size)));

    return tabMagnitude[a + t*size];
}
#endif
