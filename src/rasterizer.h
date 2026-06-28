#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include "mesh.h"
#include "image.h"
#include "shader_dispatch.h"

struct RenderContext {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
    LightParams light;
};

// Run the full rasterization pipeline on a mesh and write the result into img.
void renderMesh(const Mesh& mesh, const RenderContext& ctx, Image& img);
