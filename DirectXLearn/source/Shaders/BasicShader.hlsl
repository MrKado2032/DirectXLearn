struct VSOutput
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
};

struct PSOutput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

cbuffer mat : register(b0)
{
    matrix mat;
}

PSOutput VSMain(VSOutput vsout)
{
    PSOutput psout;
    psout.pos = mul(float4(vsout.pos, 1.0), mat);
    psout.uv = vsout.uv;
    return psout;
}

Texture2D g_Texture : register(t0);
SamplerState g_Sampler : register(s0);

float4 PSMain(PSOutput psout) : SV_Target
{
    return g_Texture.Sample(g_Sampler, psout.uv);
}