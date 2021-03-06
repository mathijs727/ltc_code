#include "ltc/plot.h"
#include "LTC.h"
#include "ltc/brdf.h"
#include <CImg.h>
#include <cmath>
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>
#include <iomanip>
#include <sstream>
// NOTE(Mathijs): replace by C++20 format when it is more widely supported.
#include <fmt/format.h>

namespace ltc {

cimg_library::CImg<float> colorMap(33, 1, 1, 3);

const unsigned char colorMap_data[33 * 3] = {
    59, 76, 192,
    68, 90, 204,
    77, 104, 215,
    87, 117, 225,
    98, 130, 234,
    108, 142, 241,
    119, 154, 247,
    130, 165, 251,
    141, 176, 254,
    152, 185, 255,
    163, 194, 255,
    174, 201, 253,
    184, 208, 249,
    194, 213, 244,
    204, 217, 238,
    213, 219, 230,
    221, 221, 221,
    229, 216, 209,
    236, 211, 197,
    241, 204, 185,
    245, 196, 173,
    247, 187, 160,
    247, 177, 148,
    247, 166, 135,
    244, 154, 123,
    241, 141, 111,
    236, 127, 99,
    229, 112, 88,
    222, 96, 77,
    213, 80, 66,
    203, 62, 56,
    192, 40, 47,
    180, 4, 38
};

class BrdfOrLTC {
public:
    BrdfOrLTC(
        const LTC* ltc_,
        const Brdf* brdf_,
        const glm::vec3 V_ = glm::vec3(0, 0, 1),
        const float alpha_ = 1.0f)
        : ltc(ltc_)
        , brdf(brdf_)
        , V(V_)
        , alpha(alpha_)
    {
        isBrdf = brdf != nullptr;
    }

    float eval(const glm::vec3& L) const
    {
        if (isBrdf) {
            float pdf;
            return brdf->eval(V, L, alpha, pdf);
        } else
            return ltc->eval(L);
    }

    glm::vec3 sample(const float U1, const float U2) const
    {
        return isBrdf ? brdf->sample(V, alpha, U1, U2) : ltc->sample(U1, U2);
    }

    float computeMaxValue() const
    {
        float max_value = 0.0;

        const int Nsample = 50;
        for (int j = 0; j < Nsample; ++j)
            for (int i = 0; i < Nsample; ++i) {
                const float U1 = (i + 0.5f) / Nsample;
                const float U2 = (j + 0.5f) / Nsample;
                max_value = std::max<float>(max_value, eval(sample(U1, U2)));
            }

        return max_value;
    }

    bool isBrdf;
    const Brdf* brdf;
    const glm::vec3 V;
    const float alpha;
    const LTC* ltc;
};

// raytrace a sphere
// evaluate the BRDF or the LTC
// call the color map
void spherical_plot(const BrdfOrLTC& brdforltc, const std::filesystem::path& savePath)
{
    // image
    const int image_size = 256;
    cimg_library::CImg<float> image(image_size, image_size, 1, 3);

    // camera
    glm::vec3 Z = glm::normalize(glm::vec3(-1, 1, 1));
    glm::vec3 X = glm::normalize(glm::vec3(1, 1, 0));
    X = glm::normalize(X - Z * glm::dot(X, Z));
    glm::vec3 Y = -cross(X, Z);

    // maximum value of the function (color map scaling)
    float max_value = brdforltc.computeMaxValue();

    // loop over pixels
    for (int j = 0; j < image_size; ++j)
        for (int i = 0; i < image_size; ++i) {
            // intersection point on the sphere
            const float x = -1.1f + 2.2f * (i + 0.5f) / image_size;
            const float y = -1.1f + 2.2f * (j + 0.5f) / image_size;
            if (x * x + y * y > 1.0f) {
                image(i, j, 0, 0) = 255.0f;
                image(i, j, 0, 1) = 255.0f;
                image(i, j, 0, 2) = 255.0f;
                continue;
            }
            const float z = std::sqrt(1.0f - x * x - y * y);
            glm::vec3 L = x * X + y * Y + z * Z;

            // evaluate function
            float value = brdforltc.eval(L) / max_value;

            // color map
            image(i, j, 0, 0) = colorMap.linear_atX(value * (colorMap.width() - 1.0f), 0, 0, 0);
            image(i, j, 0, 1) = colorMap.linear_atX(value * (colorMap.width() - 1.0f), 0, 0, 1);
            image(i, j, 0, 2) = colorMap.linear_atX(value * (colorMap.width() - 1.0f), 0, 0, 2);
        }

    const std::string savePathString = savePath.string();
    image.save(savePathString.c_str());
}

void make_spherical_plots(
    const Brdf& brdf, const glm::mat3* tab, const int N,
    const std::filesystem::path& outFolder)
{
    // init color map texture (for linear interpolation)
    for (int i = 0; i < 33; ++i) {
        colorMap(i, 0, 0, 0) = colorMap_data[3 * i + 0];
        colorMap(i, 0, 0, 1) = colorMap_data[3 * i + 1];
        colorMap(i, 0, 0, 2) = colorMap_data[3 * i + 2];
    }

    // fill LTC matrices in texture (for linear interpolation)
    cimg_library::CImg<float> LTC_matrices(N, N, 1, 9);
    for (int j = 0; j < N; ++j)
        for (int i = 0; i < N; ++i) {
            LTC_matrices(i, j, 0, 0) = tab[i + j * N][0][0];
            LTC_matrices(i, j, 0, 1) = tab[i + j * N][0][1];
            LTC_matrices(i, j, 0, 2) = tab[i + j * N][0][2];
            LTC_matrices(i, j, 0, 3) = tab[i + j * N][1][0];
            LTC_matrices(i, j, 0, 4) = tab[i + j * N][1][1];
            LTC_matrices(i, j, 0, 5) = tab[i + j * N][1][2];
            LTC_matrices(i, j, 0, 6) = tab[i + j * N][2][0];
            LTC_matrices(i, j, 0, 7) = tab[i + j * N][2][1];
            LTC_matrices(i, j, 0, 8) = tab[i + j * N][2][2];
        }

    // render spherical plots
    float alpha_tab[] = { 0.05f, 0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 1.0f };
    float theta_tab[] = { 0.0f, 15.0f, 30.0f, 45.0f, 60.0f, 75.0f, 89.0f };
    for (int a = 0; a < 7; ++a)
        for (int t = 0; t < 7; ++t) {
            // configuration
            const float alpha = alpha_tab[a];
            const float theta = theta_tab[t] * 3.14159f / 180.0f;
            const glm::vec3 V(std::sin(theta), 0.0f, std::cos(theta));

            // fetch texture with parameterization = [(std::sqrt(alpha), sqrt(1 - std::cos(theta))]
            float x = std::sqrt(alpha) * (LTC_matrices.width() - 1.0f);
            float y = std::sqrt(1.0f - V.z) * (LTC_matrices.height() - 1.0f);
            glm::mat3 M = glm::mat3(
                LTC_matrices.linear_atXY(x, y, 0, 0),
                LTC_matrices.linear_atXY(x, y, 0, 1),
                LTC_matrices.linear_atXY(x, y, 0, 2),
                LTC_matrices.linear_atXY(x, y, 0, 3),
                LTC_matrices.linear_atXY(x, y, 0, 4),
                LTC_matrices.linear_atXY(x, y, 0, 5),
                LTC_matrices.linear_atXY(x, y, 0, 6),
                LTC_matrices.linear_atXY(x, y, 0, 7),
                LTC_matrices.linear_atXY(x, y, 0, 8));

            // init LTC
            LTC ltc;
            ltc.M = M;
            ltc.invM = inverse(M);
            ltc.detM = abs(glm::determinant(M));

            const auto filePathLTC = outFolder / fmt::format("alpha_{:03}_theta_{:02}_ltc.bmp", (int)(alpha_tab[a] * 100.0f), (int)theta_tab[t]);
            const auto filePathBRDF = outFolder / fmt::format("alpha_{:03}_theta_{:02}_brdf.bmp", (int)(alpha_tab[a] * 100.0f), (int)theta_tab[t]);

            // plot LTC
            spherical_plot(BrdfOrLTC(&ltc, NULL), filePathLTC);

            // plot BRDF
            spherical_plot(BrdfOrLTC(NULL, &brdf, V, alpha), filePathBRDF);
        }
}

}
