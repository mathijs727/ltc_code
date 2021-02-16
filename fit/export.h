#pragma once

// export data to C
void writeTabC(glm::mat3* tab, glm::vec2* tabMagFresnel, int N)
{
    std::ofstream file("results/ltc.inc");

    file << std::fixed;
    file << std::setprecision(6);

    file << "static const int size = " << N << ";" << std::endl
         << std::endl;

    file << "static const mat33 tabM[size*size] = {" << std::endl;
    for (int t = 0; t < N; ++t) {
        for (int a = 0; a < N; ++a) {
            file << "{";
            file << tab[a + t * N][0][0] << ", " << tab[a + t * N][0][1] << ", " << tab[a + t * N][0][2] << ", ";
            file << tab[a + t * N][1][0] << ", " << tab[a + t * N][1][1] << ", " << tab[a + t * N][1][2] << ", ";
            file << tab[a + t * N][2][0] << ", " << tab[a + t * N][2][1] << ", " << tab[a + t * N][2][2] << "}";
            if (a != N - 1 || t != N - 1)
                file << ", ";
            file << std::endl;
        }
    }
    file << "};" << std::endl
         << std::endl;

    file << "static const mat33 tabMinv[size*size] = {" << std::endl;
    for (int t = 0; t < N; ++t) {
        for (int a = 0; a < N; ++a) {
            glm::mat3 Minv = glm::inverse(tab[a + t * N]);

            file << "{";
            file << Minv[0][0] << ", " << Minv[0][1] << ", " << Minv[0][2] << ", ";
            file << Minv[1][0] << ", " << Minv[1][1] << ", " << Minv[1][2] << ", ";
            file << Minv[2][0] << ", " << Minv[2][1] << ", " << Minv[2][2] << "}";
            if (a != N - 1 || t != N - 1)
                file << ", ";
            file << std::endl;
        }
    }
    file << "};" << std::endl
         << std::endl;

    file << "static const float tabMagnitude[size*size] = {" << std::endl;
    for (int t = 0; t < N; ++t) {
        for (int a = 0; a < N; ++a) {
            file << tabMagFresnel[a + t * N][0] << "f";
            if (a != N - 1 || t != N - 1)
                file << ", ";
            file << std::endl;
        }
    }
    file << "};" << std::endl;

    file.close();
}

// export data to MATLAB
void writeTabMatlab(glm::mat3* tab, glm::vec2* tabMagFresnel, int N)
{
    std::ofstream file("results/ltc.mat");

    file << "# name: tabMagnitude" << std::endl;
    file << "# type: matrix" << std::endl;
    file << "# ndims: 2" << std::endl;
    file << " " << N << " " << N << std::endl;

    for (int t = 0; t < N; ++t) {
        for (int a = 0; a < N; ++a) {
            file << tabMagFresnel[a + t * N][0] << " ";
        }
        file << std::endl;
    }

    for (int row = 0; row < 3; ++row) {
        for (int column = 0; column < 3; ++column) {

            file << "# name: tab" << column << row << std::endl;
            file << "# type: matrix" << std::endl;
            file << "# ndims: 2" << std::endl;
            file << " " << N << " " << N << std::endl;

            for (int t = 0; t < N; ++t) {
                for (int a = 0; a < N; ++a)
                    file << tab[a + t * N][column][row] << " ";
                file << std::endl;
            }

            file << std::endl;
        }
    }

    file.close();
}

// export data to DDS
#include "dds.h"
#include "float_to_half.h"

void writeDDS(const char* path, float* data, int N)
{
    int numTerms = N * N * 4;

    uint16_t* half = new uint16_t[numTerms];

    for (int i = 0; i < numTerms; ++i) {
        half[i] = float_to_half_fast(data[i]);
    }

    SaveDDS(path, DDS_FORMAT_R16G16B16A16_FLOAT, sizeof(uint16_t) * 4, N, N, (void const*)half);

    delete[] half;
}

void writeDDS(glm::vec4* data1, glm::vec4* data2, int N)
{
    writeDDS("results/ltc_1.dds", &data1[0][0], N);
    writeDDS("results/ltc_2.dds", &data2[0][0], N);
}

// export data to Javascript
void writeJS(glm::vec4* data1, glm::vec4* data2, int N)
{
    std::ofstream file("results/ltc.js");

    file << "var g_ltc_1 = [" << std::endl;

    for (int i = 0; i < N * N; ++i) {
        // store the variable terms
        file << data1[i].x << ", ";
        file << data1[i].y << ", ";
        file << data1[i].z << ", ";
        file << data1[i].w << ", " << std::endl;
    }
    file << "];" << std::endl;

    file << "var g_ltc_2 = [";
    for (int i = 0; i < N * N; ++i) {
        file << data2[i].x << ", ";
        file << data2[i].y << ", ";
        file << data2[i].z << ", ";
        file << data2[i].w << ", " << std::endl;
    }
    file << "];" << std::endl;

    file.close();
}
