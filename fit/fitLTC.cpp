// fitLTC.cpp : Defines the entry point for the console application.
//
#include <glm/glm.hpp>

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <vector>

#include "LTC.h"
#include "brdf.h"
#include "brdf_beckmann.h"
#include "brdf_disneyDiffuse.h"
#include "brdf_ggx.h"

#include "nelder_mead.h"

#include "export.h"
#include "plot.h"

// size of precomputed table (theta, alpha)
const int N = 64;
// number of samples used to compute the error during fitting
const int Nsample = 32;
// minimal roughness (avoid singularities)
const float MIN_ALPHA = 0.00001f;

const float pi = std::acos(-1.0f);

namespace ltc {

// computes
// * the norm (albedo) of the BRDF
// * the average Schlick Fresnel value
// * the average direction of the BRDF
void computeAvgTerms(const Brdf& brdf, const glm::vec3& V, const float alpha,
    float& norm, float& fresnel, glm::vec3& averageDir)
{
    norm = 0.0f;
    fresnel = 0.0f;
    averageDir = glm::vec3(0, 0, 0);

    for (int j = 0; j < Nsample; ++j) {
        for (int i = 0; i < Nsample; ++i) {
            const float U1 = (i + 0.5f) / Nsample;
            const float U2 = (j + 0.5f) / Nsample;

            // sample
            const glm::vec3 L = brdf.sample(V, alpha, U1, U2);

            // eval
            float pdf;
            float eval = brdf.eval(V, L, alpha, pdf);

            if (pdf > 0) {
                float weight = eval / pdf;

                glm::vec3 H = glm::normalize(V + L);

                // accumulate
                norm += weight;
                fresnel += weight * std::pow(1.0f - glm::max(glm::dot(V, H), 0.0f), 5.0f);
                averageDir += weight * L;
            }
        }
    }

    norm /= (float)(Nsample * Nsample);
    fresnel /= (float)(Nsample * Nsample);

    // clear y component, which should be zero with isotropic BRDFs
    averageDir.y = 0.0f;

    averageDir = glm::normalize(averageDir);
}

// compute the error between the BRDF and the LTC
// using Multiple Importance Sampling
float computeError(const LTC& ltc, const Brdf& brdf, const glm::vec3& V, const float alpha)
{
    double error = 0.0;

    for (int j = 0; j < Nsample; ++j) {
        for (int i = 0; i < Nsample; ++i) {
            const float U1 = (i + 0.5f) / Nsample;
            const float U2 = (j + 0.5f) / Nsample;

            // importance sample LTC
            {
                // sample
                const glm::vec3 L = ltc.sample(U1, U2);

                float pdf_brdf;
                float eval_brdf = brdf.eval(V, L, alpha, pdf_brdf);
                float eval_ltc = ltc.eval(L);
                float pdf_ltc = eval_ltc / ltc.magnitude;

                // error with MIS weight
                double error_ = std::abs(eval_brdf - eval_ltc);
                error_ = error_ * error_ * error_;
                error += error_ / (pdf_ltc + pdf_brdf);
            }

            // importance sample BRDF
            {
                // sample
                const glm::vec3 L = brdf.sample(V, alpha, U1, U2);

                float pdf_brdf;
                float eval_brdf = brdf.eval(V, L, alpha, pdf_brdf);
                float eval_ltc = ltc.eval(L);
                float pdf_ltc = eval_ltc / ltc.magnitude;

                // error with MIS weight
                double error_ = std::abs(eval_brdf - eval_ltc);
                error_ = error_ * error_ * error_;
                error += error_ / (pdf_ltc + pdf_brdf);
            }
        }
    }

    return (float)error / (float)(Nsample * Nsample);
}

struct FitLTC {
    FitLTC(LTC& ltc_, const Brdf& brdf, bool isotropic_, const glm::vec3& V_, float alpha_)
        : ltc(ltc_)
        , brdf(brdf)
        , V(V_)
        , alpha(alpha_)
        , isotropic(isotropic_)
    {
    }

    void update(const float* params)
    {
        float m11 = std::max<float>(params[0], 1e-7f);
        float m22 = std::max<float>(params[1], 1e-7f);
        float m13 = params[2];

        if (isotropic) {
            ltc.m11 = m11;
            ltc.m22 = m11;
            ltc.m13 = 0.0f;
        } else {
            ltc.m11 = m11;
            ltc.m22 = m22;
            ltc.m13 = m13;
        }
        ltc.update();
    }

    float operator()(const float* params)
    {
        update(params);
        return computeError(ltc, brdf, V, alpha);
    }

    const Brdf& brdf;
    LTC& ltc;
    bool isotropic;

    const glm::vec3& V;
    float alpha;
};

// fit brute force
// refine first guess by exploring parameter space
void fit(LTC& ltc, const Brdf& brdf, const glm::vec3& V, const float alpha, const float epsilon = 0.05f, const bool isotropic = false)
{
    float startFit[3] = { ltc.m11, ltc.m22, ltc.m13 };
    float resultFit[3];

    FitLTC fitter(ltc, brdf, isotropic, V, alpha);

    // Find best-fit LTC lobe (scale, alphax, alphay)
    float error = NelderMead<3>(resultFit, startFit, epsilon, 1e-5f, 100, fitter);

    // Update LTC with best fitting values
    fitter.update(resultFit);
}

// fit data
void fitTab(glm::mat3* tab, glm::vec2* tabMagFresnel, const int N, const Brdf& brdf)
{
    LTC ltc;

    // loop over theta and alpha
    for (int a = N - 1; a >= 0; --a) {
        for (int t = 0; t <= N - 1; ++t) {
            // parameterized by sqrt(1 - cos(theta))
            float x = t / float(N - 1);
            float ct = 1.0f - x * x;
            float theta = std::min<float>(1.57f, std::acos(ct)); // 1.57 ~= pi/2
            const glm::vec3 V = glm::vec3(std::sin(theta), 0, std::cos(theta));

            // alpha = roughness^2
            float roughness = a / float(N - 1);
            float alpha = std::max<float>(roughness * roughness, MIN_ALPHA);

            std::cout << "a = " << a << "\t t = " << t << std::endl;
            std::cout << "alpha = " << alpha << "\t theta = " << theta << std::endl;
            std::cout << std::endl;

            glm::vec3 averageDir;
            computeAvgTerms(brdf, V, alpha, ltc.magnitude, ltc.fresnel, averageDir);

            bool isotropic;

            // 1. first guess for the fit
            // init the hemisphere in which the distribution is fitted
            // if theta == 0 the lobe is rotationally symmetric and aligned with Z = (0 0 1)
            if (t == 0) {
                ltc.X = glm::vec3(1, 0, 0);
                ltc.Y = glm::vec3(0, 1, 0);
                ltc.Z = glm::vec3(0, 0, 1);

                if (a == N - 1) // roughness = 1
                {
                    ltc.m11 = 1.0f;
                    ltc.m22 = 1.0f;
                } else // init with roughness of previous fit
                {
                    ltc.m11 = tab[a + 1 + t * N][0][0];
                    ltc.m22 = tab[a + 1 + t * N][1][1];
                }

                ltc.m13 = 0;
                ltc.update();

                isotropic = true;
            }
            // otherwise use previous configuration as first guess
            else {
                glm::vec3 L = averageDir;
                glm::vec3 T1(L.z, 0, -L.x);
                glm::vec3 T2(0, 1, 0);
                ltc.X = T1;
                ltc.Y = T2;
                ltc.Z = L;

                ltc.update();

                isotropic = false;
            }

            // 2. fit (explore parameter space and refine first guess)
            float epsilon = 0.05f;
            fit(ltc, brdf, V, alpha, epsilon, isotropic);

            // copy data
            tab[a + t * N] = ltc.M;
            tabMagFresnel[a + t * N][0] = ltc.magnitude;
            tabMagFresnel[a + t * N][1] = ltc.fresnel;

            // kill useless coefs in matrix
            tab[a + t * N][0][1] = 0;
            tab[a + t * N][1][0] = 0;
            tab[a + t * N][2][1] = 0;
            tab[a + t * N][1][2] = 0;

            std::cout << tab[a + t * N][0][0] << "\t " << tab[a + t * N][1][0] << "\t " << tab[a + t * N][2][0] << std::endl;
            std::cout << tab[a + t * N][0][1] << "\t " << tab[a + t * N][1][1] << "\t " << tab[a + t * N][2][1] << std::endl;
            std::cout << tab[a + t * N][0][2] << "\t " << tab[a + t * N][1][2] << "\t " << tab[a + t * N][2][2] << std::endl;
            std::cout << std::endl;
        }
    }
}

float sqr(float x)
{
    return x * x;
}

float G(float w, float s, float g)
{
    return -2.0f * std::sin(w) * std::cos(s) * std::cos(g) + pi / 2.0f - g + std::sin(g) * std::cos(g);
}

float H(float w, float s, float g)
{
    float sinsSq = sqr(sin(s));
    float cosgSq = sqr(cos(g));

    return std::cos(w) * (std::cos(g) * std::sqrt(sinsSq - cosgSq) + sinsSq * std::asin(std::cos(g) / std::sin(s)));
}

float ihemi(float w, float s)
{
    float g = std::asin(std::cos(s) / std::sin(w));
    float sinsSq = sqr(std::sin(s));

    if (w >= 0.0f && w <= (pi / 2.0f - s))
        return pi * std::cos(w) * sinsSq;

    if (w >= (pi / 2.0f - s) && w < pi / 2.0f)
        return pi * std::cos(w) * sinsSq + G(w, s, g) - H(w, s, g);

    if (w >= pi / 2.0f && w < (pi / 2.0f + s))
        return G(w, s, g) + H(w, s, g);

    return 0.0f;
}

void genSphereTab(float* tabSphere, int N)
{
    for (int j = 0; j < N; ++j) {
        for (int i = 0; i < N; ++i) {
            const float U1 = float(i) / (N - 1);
            const float U2 = float(j) / (N - 1);

            // z = cos(elevation angle)
            float z = 2.0f * U1 - 1.0f;

            // length of average dir., proportional to sin(sigma)^2
            float len = U2;

            float sigma = std::asin(std::sqrt(len));
            float omega = std::acos(z);

            // compute projected (cosine-weighted) solid angle of spherical cap
            float value = 0.0f;

            if (sigma > 0.0f)
                value = ihemi(omega, sigma) / (pi * len);
            else
                value = std::max<float>(z, 0.0f);

            if (value != value)
                printf("nan!\n");

            tabSphere[i + j * N] = value;
        }
    }
}

void packTab(
    glm::vec4* tex1, glm::vec4* tex2,
    const glm::mat3* tab,
    const glm::vec2* tabMagFresnel,
    const float* tabSphere,
    int N)
{
    for (int i = 0; i < N * N; ++i) {
        const glm::mat3& m = tab[i];

        glm::mat3 invM = inverse(m);

        // glm::normalize by the middle element
        invM /= invM[1][1];

        // store the variable terms
        tex1[i].x = invM[0][0];
        tex1[i].y = invM[0][2];
        tex1[i].z = invM[2][0];
        tex1[i].w = invM[2][2];
        tex2[i].x = tabMagFresnel[i][0];
        tex2[i].y = tabMagFresnel[i][1];
        tex2[i].z = 0.0f; // unused
        tex2[i].w = tabSphere[i];
    }
}

}

int main(int argc, char* argv[])
{
    using namespace ltc;

    // BRDF to fit
    BrdfGGX brdf;
    //BrdfBeckmann brdf;
    //BrdfDisneyDiffuse brdf;

    // allocate data
    std::vector<glm::mat3> tab(N * N);
    std::vector<glm::vec2> tabMagFresnel(N * N);
    std::vector<float> tabSphere(N * N);

    // fit
    fitTab(tab.data(), tabMagFresnel.data(), N, brdf);

    // projected solid angle of a spherical cap, clipped to the horizon
    genSphereTab(tabSphere.data(), N);

    // pack tables (texture representation)
    std::vector<glm::vec4> tex1(N * N);
    std::vector<glm::vec4> tex2(N * N);
    packTab(tex1.data(), tex2.data(), tab.data(), tabMagFresnel.data(), tabSphere.data(), N);

    // export to C, MATLAB and DDS
    writeTabMatlab(tab.data(), tabMagFresnel.data(), N);
    writeTabC(tab.data(), tabMagFresnel.data(), N);
    writeDDS(tex1.data(), tex2.data(), N);
    writeJS(tex1.data(), tex2.data(), N);

    // spherical plots
    // make_spherical_plots(brdf, tab, N);

    return 0;
}


