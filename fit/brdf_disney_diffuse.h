#pragma once
#include "brdf.h"

namespace ltc {

class BrdfDisneyDiffuse : public Brdf {
public:
    float eval(const glm::vec3& V, const glm::vec3& L, const float alpha, float& pdf) const override;
    virtual glm::vec3 sample(const glm::vec3& V, const float alpha, const float U1, const float U2) const override;
};

}
