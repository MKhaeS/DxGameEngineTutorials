
struct VS_OUTPUT
{
    float4 position         : SV_POSITION;
};


float4 main(VS_OUTPUT input) : SV_TARGET
{
    float4 color = float4(0.2, 0.8, 0.4, 1.0);
    return color;
}