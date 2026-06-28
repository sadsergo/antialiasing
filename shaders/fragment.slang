struct LightUniforms
{
    float3 lightPos;
    float3 lightColor;
    float3 viewPos;
};

struct FragIn
{
    float3 worldPos;
    float3 worldNormal;
    float3 baseColor;
};

struct FragOut
{
    float4 color;
};

RWStructuredBuffer<FragIn>    gFragIn;
RWStructuredBuffer<FragOut>   gFragOut;
ConstantBuffer<LightUniforms> gLightUniforms;

[shader("compute")]
[numthreads(64, 1, 1)]
void fragmentMain(uint3 tid : SV_DispatchThreadID, uniform uint fragCount)
{
    if (tid.x >= fragCount)
        return;

    FragIn f = gFragIn[tid.x];

    float3 n = normalize(f.worldNormal);
    float3 l = normalize(gLightUniforms.lightPos - f.worldPos);
    float3 v = normalize(gLightUniforms.viewPos  - f.worldPos);
    float3 r = reflect(-l, n);

    float3 ambient  = 0.15f * f.baseColor;
    float3 diffuse  = max(dot(n, l), 0.0f) * gLightUniforms.lightColor * f.baseColor;
    float3 specular = pow(max(dot(v, r), 0.0f), 64.0f) * gLightUniforms.lightColor * 0.5f;

    float3 col = clamp(ambient + diffuse + specular, float3(0.0f), float3(1.0f));
    gFragOut[tid.x].color = float4(col, 1.0f);
}
