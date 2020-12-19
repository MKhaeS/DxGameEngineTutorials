struct VS_INPUT
{
    float3 pos      : POSITION;
    float2 uv       : TEXCOORD;
};

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
    charProp cP[100]; // -- !!! Get from file
};


VS_OUTPUT main(VS_INPUT input, uint instancePos : INSTANCEPOS, uint  instanceid : SV_InstanceID)
{       
    charProp cprop = cP[instanceid+instancePos];
    VS_OUTPUT result;
    input.pos.xy *= cprop.size.xy * cprop.fontScale;
    input.pos.xy += cprop.offset.xy * cprop.fontScale + cprop.position;
    result.position = float4(input.pos, 1.0);
    result.texcoord = input.uv;
    result.instance = instanceid;
    result.instance_pos = instancePos;
    return result;
}