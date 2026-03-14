struct VSOutput
{
    float3 pos : POSITION;
    float4 col : COLOR;
};

struct PSOutput
{
    float4 pos : SV_POSITION;
    float4 col : COLOR;
};

PSOutput VSMain(VSOutput vsout)
{
    PSOutput psout;
    psout.pos = float4(vsout.pos, 1.0);
    psout.col = vsout.col;
    return psout;
}

float4 PSMain(PSOutput psout) : SV_Target
{
    return psout.col;
}