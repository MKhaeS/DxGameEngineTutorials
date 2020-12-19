struct VS_INPUT {
    float2 position      : POSITION;
};

struct VS_OUTPUT {
    float4 position         : SV_POSITION;
};

cbuffer screen: register (b1) {
    int width;
    int height;    
}

cbuffer geometry: register (b2) {
    int2 position;
    int2 size;
}


VS_OUTPUT main(VS_INPUT input) {       
    VS_OUTPUT result;
    float2 screen_size = float2(width, height);
    float2 offset = position / (screen_size / 2) - 1.0;
    offset.y = -offset.y;    
    float2 screen_pos = float2(input.position) * size;
    screen_pos /= screen_size/2;
    screen_pos += offset;
    result.position = float4(screen_pos, 1.0, 1.0);
    return result;
}