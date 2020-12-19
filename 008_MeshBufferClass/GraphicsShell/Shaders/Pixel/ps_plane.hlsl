Texture2D    t1[2] : register(t0);
SamplerState s1 : register(s0);


struct VS_OUTPUT
{    
    
    float4 position_sc      : SV_POSITION;
    //float3 normal           : NORMAL;    
    float2 uv               : TEXCOORD;
    uint   instanceId       : INSTANCE_ID;
};


float4 main(VS_OUTPUT input) : SV_TARGET
{
    float4 color = t1[input.instanceId].Sample( s1, input.uv);
    //float4 color = float4(0.5*input.normal+0.5,1.0);
    //float4 color = float4(input.pos_wd, 1.0);
    //float4 color = 0.5;
    return color;
}