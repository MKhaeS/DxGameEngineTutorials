Texture2D    t1[2] : register(t0);
SamplerState s1 : register(s0);


struct VS_OUTPUT
{
    float4 position         : SV_POSITION;
    float2 uv               : TEXCOORD;
    uint   instanceId       : INSTANCE_ID;
};


float4 main(VS_OUTPUT input) : SV_TARGET
{
    float4 color = t1[input.instanceId].Sample( s1, input.uv);
    //float4 color = float4(input.uv, 0.0, 1.0);
    return float4(color.xyz, 1.0);
}