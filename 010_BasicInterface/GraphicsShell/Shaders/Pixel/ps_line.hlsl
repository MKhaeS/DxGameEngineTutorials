struct VS_OUTPUT
{    
    float4 position_sc      : SV_POSITION;
    float3 color            : COLOR;
};


float4 main(VS_OUTPUT input) : SV_TARGET
{
    float4 color = float4(input.color, 1.0);
    return color;
}