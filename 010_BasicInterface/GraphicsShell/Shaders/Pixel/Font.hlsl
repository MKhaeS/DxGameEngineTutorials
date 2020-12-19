Texture2D    t1 : register(t0);
SamplerState s1 : register(s0);

struct VS_OUTPUT
{
    float4 position         : SV_POSITION;
    float2 texcoord         : TEXCOORD;
    int    instance         : INSTANCE_ID;  
    int    instance_pos     : INSTANCE_POS;
};

struct charProp {
    float2 offset;
    float2 size;
    float2 uvOffset;
    float2 uvScale;
    float4 color;
    float2 position;
    float  fontScale;    
    int    instanceOffset;
};

cbuffer PropertiesBuffer : register(b0) {
    charProp cP[1000];
};

//[RootSignature(MyRS1)]
float4 main(VS_OUTPUT input) : SV_TARGET
{
    float4 tx_sample;
    charProp cprop = cP[input.instance+input.instance_pos];
    tx_sample = t1.Sample(s1, cprop.uvScale*input.texcoord + cprop.uvOffset);
    if(tx_sample.r < 0.3)
        discard;
    return cprop.color*tx_sample;
    //return float4(0.2f, 0.8f, 0.3f, 0.0f); // Red, Green, Blue, Alpha
}