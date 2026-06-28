struct Uniforms
{
    float4x4 mvp;
    float4x4 model;
};

struct VSIn
{
    float3 position;
    float3 normal;
    float3 color;
};

struct VSOut
{
    float4 clipPos;
    float3 worldPos;
    float3 worldNormal;
    float3 color;
};

RWStructuredBuffer<VSIn>  gVertexIn;
RWStructuredBuffer<VSOut> gVertexOut;
ConstantBuffer<Uniforms>  gUniforms;

[shader("compute")]
[numthreads(64, 1, 1)]
void vertexMain(uint3 tid : SV_DispatchThreadID, uniform uint vertexCount)
{
    if (tid.x >= vertexCount)
        return;

    VSIn  v = gVertexIn[tid.x];
    VSOut o;

    float4 worldPos4  = mul(gUniforms.model, float4(v.position, 1.0f));
    o.worldPos        = worldPos4.xyz;
    o.worldNormal     = normalize(mul((float3x3)gUniforms.model, v.normal));
    o.clipPos         = mul(gUniforms.mvp, float4(v.position, 1.0f));
    o.color           = v.color;

    gVertexOut[tid.x] = o;
}
