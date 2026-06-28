// Slang-generated C++ is compiled as separate translation units.
// Here we declare the generated dispatch functions and build the data structures
// that match the layout in gen/vertex_gen.cpp and gen/fragment_gen.cpp.

#include "shader_dispatch.h"
#include <glm/gtc/type_ptr.hpp>
#include <cstring>
#include <cstdint>

// Slang prelude types used by the dispatch functions
#include <slang-cpp-types.h>

// ── Memory-layout mirrors of the generated structs ──────────────────────────
// These match the layout emitted by slangc exactly (verified from gen/ files).

struct SL_Vec3  { float x, y, z; };
struct SL_Vec4  { float x, y, z, w; };

// _MatrixStorage_float4x4_ColMajornatural_0: 4 column-vectors of float4 = 64 bytes
// GLM mat4 has identical column-major layout, so we can memcpy directly.
struct SL_Mat4  { float col[4][4]; };  // [column][row]

// VSIn_0
struct SL_VSIn  { SL_Vec3 position; SL_Vec3 normal; SL_Vec3 color; };
// VSOut_0
struct SL_VSOut { SL_Vec4 clipPos; SL_Vec3 worldPos; SL_Vec3 worldNormal; SL_Vec3 color; };
// Uniforms_natural_0
struct SL_Uniforms { SL_Mat4 mvp; SL_Mat4 model; };

// FragIn_0
struct SL_FragIn  { SL_Vec3 worldPos; SL_Vec3 worldNormal; SL_Vec3 baseColor; };
// FragOut_0
struct SL_FragOut { SL_Vec4 color; };
// LightUniforms_0
struct SL_LightUniforms { SL_Vec3 lightPos; SL_Vec3 lightColor; SL_Vec3 viewPos; };

// RWStructuredBuffer<T> layout: { T* data; size_t count; }
template<typename T>
struct SL_Buffer { T* data; size_t count; };

// GlobalParams_0 (vertex shader)
struct VS_GlobalParams {
    SL_Buffer<SL_VSIn>  gVertexIn_0;
    SL_Buffer<SL_VSOut> gVertexOut_0;
    SL_Uniforms*        gUniforms_0;
};
struct VS_EntryParams { uint32_t vertexCount_0; };

// GlobalParams_0 (fragment shader)
struct FS_GlobalParams {
    SL_Buffer<SL_FragIn>  gFragIn_0;
    SL_Buffer<SL_FragOut> gFragOut_0;
    SL_LightUniforms*     gLightUniforms_0;
};
struct FS_EntryParams { uint32_t fragCount_0; };

// ── Forward-declare the Slang-generated entry-points (extern "C" from SLANG_PRELUDE_EXPORT)
extern "C" {
    void vertexMain  (ComputeVaryingInput*, void* ep, void* gp);
    void fragmentMain(ComputeVaryingInput*, void* ep, void* gp);
}

// ── Helper: fill SL_Mat4 from glm::mat4 (both are column-major, direct copy) ─
static void toSLMat(SL_Mat4& dst, const glm::mat4& src) {
    std::memcpy(&dst, glm::value_ptr(src), sizeof(SL_Mat4));
}

// ── Vertex shader dispatch ───────────────────────────────────────────────────
void runVertexShader(
    const std::vector<Vertex>& verts,
    const glm::mat4& mvp,
    const glm::mat4& model,
    std::vector<VSOutput>& out)
{
    uint32_t count = static_cast<uint32_t>(verts.size());
    out.resize(count);

    std::vector<SL_VSIn>  sins(count);
    std::vector<SL_VSOut> souts(count);

    for (uint32_t i = 0; i < count; ++i) {
        sins[i].position = {verts[i].pos.x,    verts[i].pos.y,    verts[i].pos.z};
        sins[i].normal   = {verts[i].normal.x,  verts[i].normal.y, verts[i].normal.z};
        sins[i].color    = {verts[i].color.x,   verts[i].color.y,  verts[i].color.z};
    }

    SL_Uniforms uniforms;
    toSLMat(uniforms.mvp,   mvp);
    toSLMat(uniforms.model, model);

    VS_GlobalParams gp;
    gp.gVertexIn_0  = {sins.data(),  count};
    gp.gVertexOut_0 = {souts.data(), count};
    gp.gUniforms_0  = &uniforms;

    VS_EntryParams ep;
    ep.vertexCount_0 = count;

    ComputeVaryingInput vi;
    vi.startGroupID = {0, 0, 0};
    vi.endGroupID   = {(count + 63u) / 64u, 1u, 1u};

    vertexMain(&vi, &ep, &gp);

    for (uint32_t i = 0; i < count; ++i) {
        out[i].clipPos    = {souts[i].clipPos.x,    souts[i].clipPos.y,    souts[i].clipPos.z,    souts[i].clipPos.w};
        out[i].worldPos   = {souts[i].worldPos.x,   souts[i].worldPos.y,   souts[i].worldPos.z};
        out[i].worldNormal= {souts[i].worldNormal.x,souts[i].worldNormal.y,souts[i].worldNormal.z};
        out[i].color      = {souts[i].color.x,      souts[i].color.y,      souts[i].color.z};
    }
}

// ── Fragment shader dispatch ─────────────────────────────────────────────────
void runFragmentShader(
    const std::vector<FragData>& frags,
    const LightParams& light,
    std::vector<glm::vec4>& colors)
{
    uint32_t count = static_cast<uint32_t>(frags.size());
    colors.resize(count);

    std::vector<SL_FragIn>  fins(count);
    std::vector<SL_FragOut> fouts(count);

    for (uint32_t i = 0; i < count; ++i) {
        fins[i].worldPos   = {frags[i].worldPos.x,    frags[i].worldPos.y,   frags[i].worldPos.z};
        fins[i].worldNormal= {frags[i].worldNormal.x,  frags[i].worldNormal.y,frags[i].worldNormal.z};
        fins[i].baseColor  = {frags[i].baseColor.x,   frags[i].baseColor.y,  frags[i].baseColor.z};
    }

    SL_LightUniforms lu;
    lu.lightPos   = {light.lightPos.x,   light.lightPos.y,   light.lightPos.z};
    lu.lightColor = {light.lightColor.x, light.lightColor.y, light.lightColor.z};
    lu.viewPos    = {light.viewPos.x,    light.viewPos.y,    light.viewPos.z};

    FS_GlobalParams gp;
    gp.gFragIn_0          = {fins.data(),  count};
    gp.gFragOut_0         = {fouts.data(), count};
    gp.gLightUniforms_0   = &lu;

    FS_EntryParams ep;
    ep.fragCount_0 = count;

    ComputeVaryingInput vi;
    vi.startGroupID = {0, 0, 0};
    vi.endGroupID   = {(count + 63u) / 64u, 1u, 1u};

    fragmentMain(&vi, &ep, &gp);

    for (uint32_t i = 0; i < count; ++i) {
        colors[i] = {fouts[i].color.x, fouts[i].color.y, fouts[i].color.z, fouts[i].color.w};
    }
}
