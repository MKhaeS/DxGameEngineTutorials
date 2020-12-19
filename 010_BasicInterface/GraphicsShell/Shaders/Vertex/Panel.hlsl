struct VS_INPUT {
    float2 position      : POSITION;
    float2 uv            : TEXCOORD;
};

struct VS_OUTPUT {
    float4 position         : SV_POSITION;
    float2 uv               : TEXCOORD;
    float4 color            : COLOR;
    uint   instanceId       : INSTANCE_ID;
};

cbuffer screen: register (b0) {
    int width;
    int height;    
}

struct Panel {
    int2   position;
    int2   size;
    float4 color;
};

cbuffer buffer1: register (b1) {
    Panel panels[10];
}


VS_OUTPUT main(VS_INPUT input, uint  instanceid : SV_InstanceID) {   
    VS_OUTPUT output;
    float2 screen_size = float2(width, height);
    float2 offset = panels[instanceid].position / (screen_size / 2) - 1.0;
    offset.y = -offset.y;    
    float2 screen_pos = float2(input.position) * panels[instanceid].size;
    screen_pos /= screen_size/2;
    screen_pos += offset;
    output.position = float4(screen_pos, 0.0, 1.0);
    output.uv       = input.uv;
    output.instanceId = instanceid;
    output.color      = panels[instanceid].color;
    return output;
}