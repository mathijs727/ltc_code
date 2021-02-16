#pragma once
#include <glm/fwd.hpp>

namespace ltc {

// export data to C
void writeTabC(glm::mat3* tab, glm::vec2* tabMagFresnel, int N);

// export data to MATLAB
void writeTabMatlab(glm::mat3* tab, glm::vec2* tabMagFresnel, int N);
void writeDDS(const char* path, float* data, int N);
void writeDDS(glm::vec4* data1, glm::vec4* data2, int N);
// export data to Javascript
void writeJS(glm::vec4* data1, glm::vec4* data2, int N);

}
