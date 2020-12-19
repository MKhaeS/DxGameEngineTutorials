struct VS_INPUT {
    float2 position      : POSITION;
    float2 uv            : TEXCOORD;
};

struct VS_OUTPUT {
    float4 position         : SV_POSITION;
    float2 uv               : TEXCOORD;
    uint   instanceId       : INSTANCE_ID;
};

cbuffer screen: register (b1) {
    int width;
    int height;    
}

cbuffer buffer1: register (b50) {
    int4 position[10];
}

/*cbuffer geometry2: register (b1000) {
    int2 size;
}*/


VS_OUTPUT main(VS_INPUT input, uint  instanceid : SV_InstanceID) {   
    int2 size = int2(100,100);
    VS_OUTPUT output;
    float2 screen_size = float2(width, height);
    float2 offset = posit3ion[instanceid].xy / (screen_size / 2) - 1.0;
    offset.y = -offset.y;    
    float2 screen_pos = float2(input.position) * size;
    screen_pos /= screen_size/2;
    screen_pos += offset;
    output.position = float4(screen_pos, 1.0, 1.0);
    output.uv       = input.uv;
    output.instanceId = instanceid;
    return output;
}