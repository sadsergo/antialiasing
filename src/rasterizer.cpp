#include "rasterizer.h"
#include <glm/glm.hpp>
#include <algorithm>
#include <vector>
#include <cstdint>
#include <limits>

// ── Screen-space vertex after perspective divide + viewport transform ─────────
struct ScreenVert {
    glm::vec2 screen; // pixel coords
    float     ndcZ;   // depth for z-buffer
    float     clipW;  // original clip-space W (for perspective-correct interp)
    glm::vec3 worldPos;
    glm::vec3 worldNormal;
    glm::vec3 color;
};

static ScreenVert toScreen(const VSOutput& v, int W, int H) {
    float w = v.clipPos.w;
    glm::vec3 ndc = glm::vec3(v.clipPos) / w;

    ScreenVert s;
    s.screen      = {(ndc.x + 1.f) * 0.5f * W, (1.f - ndc.y) * 0.5f * H};
    s.ndcZ        = ndc.z;
    s.clipW       = w;
    s.worldPos    = v.worldPos;
    s.worldNormal = v.worldNormal;
    s.color       = v.color;
    return s;
}

// ── Barycentric coordinates for point p relative to triangle (a,b,c) ─────────
static glm::vec3 bary(glm::vec2 a, glm::vec2 b, glm::vec2 c, glm::vec2 p) {
    float d = (b.y - c.y) * (a.x - c.x) + (c.x - b.x) * (a.y - c.y);
    if (std::abs(d) < 1e-6f) return {-1, -1, -1};
    float l0 = ((b.y - c.y) * (p.x - c.x) + (c.x - b.x) * (p.y - c.y)) / d;
    float l1 = ((c.y - a.y) * (p.x - c.x) + (a.x - c.x) * (p.y - c.y)) / d;
    float l2 = 1.f - l0 - l1;
    return {l0, l1, l2};
}

// ── Perspective-correct interpolation of a vec3 attribute ────────────────────
template<typename T>
static T pcInterp(const T& a0, const T& a1, const T& a2,
                  float w0, float w1, float w2,
                  float l0, float l1, float l2)
{
    float inv0 = l0 / w0, inv1 = l1 / w1, inv2 = l2 / w2;
    float invW = inv0 + inv1 + inv2;
    return (a0 * inv0 + a1 * inv1 + a2 * inv2) * (1.f / invW);
}

void renderMesh(const Mesh& mesh, const RenderContext& ctx, Image& img) {
    const int W = img.width, H = img.height;

    // ── 1. Vertex shader (Slang compute dispatch) ─────────────────────────────
    glm::mat4 vp  = ctx.proj * ctx.view;
    glm::mat4 mvp = vp * ctx.model;

    std::vector<VSOutput> vsOut;
    runVertexShader(mesh.vertices, mvp, ctx.model, vsOut);

    // ── 2. Project each vertex to screen space ────────────────────────────────
    std::vector<ScreenVert> sverts(vsOut.size());
    for (size_t i = 0; i < vsOut.size(); ++i)
        sverts[i] = toScreen(vsOut[i], W, H);

    // ── 3. Per-triangle rasterization ─────────────────────────────────────────
    // We collect all visible fragments, then dispatch the fragment shader on them.
    struct Fragment {
        int px, py;
        glm::vec3 worldPos;
        glm::vec3 worldNormal;
        glm::vec3 baseColor;
        float     depth;
        size_t    fragIdx; // index into per-pixel winner list
    };

    // Temporary per-pixel depth and fragment index
    std::vector<float>  pxDepth(W * H, std::numeric_limits<float>::infinity());
    std::vector<int>    pxFrag (W * H, -1);
    std::vector<Fragment> candidates;
    candidates.reserve(4096);

    for (size_t ti = 0; ti < mesh.indices.size(); ti += 3) {
        const ScreenVert& s0 = sverts[mesh.indices[ti + 0]];
        const ScreenVert& s1 = sverts[mesh.indices[ti + 1]];
        const ScreenVert& s2 = sverts[mesh.indices[ti + 2]];

        // Clip: reject if all vertices are behind camera
        if (s0.clipW <= 0.f && s1.clipW <= 0.f && s2.clipW <= 0.f) continue;

        // Back-face culling (signed area in screen space)
        glm::vec2 e1 = s1.screen - s0.screen;
        glm::vec2 e2 = s2.screen - s0.screen;
        float area2 = e1.x * e2.y - e1.y * e2.x;
        if (area2 >= 0.f) continue; // clockwise in screen-space = back-facing

        // Bounding box
        int xMin = std::max(0,   (int)std::floor(std::min({s0.screen.x, s1.screen.x, s2.screen.x})));
        int xMax = std::min(W-1, (int)std::ceil (std::max({s0.screen.x, s1.screen.x, s2.screen.x})));
        int yMin = std::max(0,   (int)std::floor(std::min({s0.screen.y, s1.screen.y, s2.screen.y})));
        int yMax = std::min(H-1, (int)std::ceil (std::max({s0.screen.y, s1.screen.y, s2.screen.y})));

        for (int py = yMin; py <= yMax; ++py) {
            for (int px = xMin; px <= xMax; ++px) {
                glm::vec2 p = {px + 0.5f, py + 0.5f};
                glm::vec3 l = bary(s0.screen, s1.screen, s2.screen, p);
                if (l.x < 0 || l.y < 0 || l.z < 0) continue;

                // Perspective-correct depth
                float depth = l.x * s0.ndcZ + l.y * s1.ndcZ + l.z * s2.ndcZ;
                if (depth < -1.f || depth > 1.f) continue;

                int pxi = py * W + px;
                if (depth >= pxDepth[pxi]) continue;
                pxDepth[pxi] = depth;

                // Perspective-correct interpolation of attributes
                glm::vec3 wp = pcInterp(s0.worldPos,    s1.worldPos,    s2.worldPos,
                                         s0.clipW, s1.clipW, s2.clipW, l.x, l.y, l.z);
                glm::vec3 wn = glm::normalize(
                               pcInterp(s0.worldNormal, s1.worldNormal, s2.worldNormal,
                                        s0.clipW, s1.clipW, s2.clipW, l.x, l.y, l.z));
                glm::vec3 col= pcInterp(s0.color,       s1.color,       s2.color,
                                         s0.clipW, s1.clipW, s2.clipW, l.x, l.y, l.z);

                int cidx = static_cast<int>(candidates.size());
                candidates.push_back({px, py, wp, wn, col, depth, (size_t)cidx});
                pxFrag[pxi] = cidx;
            }
        }
    }

    if (candidates.empty()) return;

    // ── 4. Fragment shader (Slang compute dispatch) ───────────────────────────
    std::vector<FragData> fragInputs(candidates.size());
    for (size_t i = 0; i < candidates.size(); ++i) {
        fragInputs[i] = {candidates[i].worldPos,
                         candidates[i].worldNormal,
                         candidates[i].baseColor};
    }

    std::vector<glm::vec4> fragColors;
    runFragmentShader(fragInputs, ctx.light, fragColors);

    // ── 5. Write surviving (depth-winning) fragments to framebuffer ───────────
    for (int py = 0; py < H; ++py) {
        for (int px = 0; px < W; ++px) {
            int fi = pxFrag[py * W + px];
            if (fi < 0) continue;
            const glm::vec4& c = fragColors[fi];
            img.setPixel(px, py, glm::vec3(c.r, c.g, c.b));
        }
    }
}
