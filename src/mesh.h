#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <vector>
#include <cstdint>

struct Vertex {
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec3 color;
};

struct Mesh {
    std::vector<Vertex>   vertices;
    std::vector<uint32_t> indices;
};

// Unit cube centered at origin, each face has a distinct color
inline Mesh makeCube() {
    const float s = 0.5f;
    Mesh m;
    m.vertices = {
        // +Z (blue)
        {{-s,-s, s},{0,0,1},{0.25f,0.45f,0.95f}},
        {{ s,-s, s},{0,0,1},{0.25f,0.45f,0.95f}},
        {{ s, s, s},{0,0,1},{0.25f,0.45f,0.95f}},
        {{-s, s, s},{0,0,1},{0.25f,0.45f,0.95f}},
        // -Z (teal)
        {{ s,-s,-s},{0,0,-1},{0.1f,0.7f,0.7f}},
        {{-s,-s,-s},{0,0,-1},{0.1f,0.7f,0.7f}},
        {{-s, s,-s},{0,0,-1},{0.1f,0.7f,0.7f}},
        {{ s, s,-s},{0,0,-1},{0.1f,0.7f,0.7f}},
        // +X (red)
        {{ s,-s, s},{1,0,0},{0.95f,0.25f,0.25f}},
        {{ s,-s,-s},{1,0,0},{0.95f,0.25f,0.25f}},
        {{ s, s,-s},{1,0,0},{0.95f,0.25f,0.25f}},
        {{ s, s, s},{1,0,0},{0.95f,0.25f,0.25f}},
        // -X (orange)
        {{-s,-s,-s},{-1,0,0},{0.95f,0.55f,0.1f}},
        {{-s,-s, s},{-1,0,0},{0.95f,0.55f,0.1f}},
        {{-s, s, s},{-1,0,0},{0.95f,0.55f,0.1f}},
        {{-s, s,-s},{-1,0,0},{0.95f,0.55f,0.1f}},
        // +Y (green)
        {{-s, s, s},{0,1,0},{0.25f,0.85f,0.35f}},
        {{ s, s, s},{0,1,0},{0.25f,0.85f,0.35f}},
        {{ s, s,-s},{0,1,0},{0.25f,0.85f,0.35f}},
        {{-s, s,-s},{0,1,0},{0.25f,0.85f,0.35f}},
        // -Y (magenta)
        {{-s,-s,-s},{0,-1,0},{0.85f,0.25f,0.75f}},
        {{ s,-s,-s},{0,-1,0},{0.85f,0.25f,0.75f}},
        {{ s,-s, s},{0,-1,0},{0.85f,0.25f,0.75f}},
        {{-s,-s, s},{0,-1,0},{0.85f,0.25f,0.75f}},
    };
    m.indices = {
         0, 1, 2,  0, 2, 3,
         4, 5, 6,  4, 6, 7,
         8, 9,10,  8,10,11,
        12,13,14, 12,14,15,
        16,17,18, 16,18,19,
        20,21,22, 20,22,23,
    };
    return m;
}
