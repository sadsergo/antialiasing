#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include "mesh.h"
#include <vector>

// Output of the vertex shader stage
struct VSOutput {
    glm::vec4 clipPos;
    glm::vec3 worldPos;
    glm::vec3 worldNormal;
    glm::vec3 color;
};

// Per-fragment data passed to the fragment shader
struct FragData {
    glm::vec3 worldPos;
    glm::vec3 worldNormal;
    glm::vec3 baseColor;
};

struct LightParams {
    glm::vec3 lightPos;
    glm::vec3 lightColor;
    glm::vec3 viewPos;
};

// Compile Slang shaders to C++ at runtime and dispatch them
void runVertexShader(
    const std::vector<Vertex>& verts,
    const glm::mat4& mvp,
    const glm::mat4& model,
    std::vector<VSOutput>& out
);

void runFragmentShader(
    const std::vector<FragData>& frags,
    const LightParams& light,
    std::vector<glm::vec4>& colors
);
