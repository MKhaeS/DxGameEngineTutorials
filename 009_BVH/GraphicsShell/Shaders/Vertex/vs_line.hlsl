struct VS_INPUT {
    float3 position      : POSITION;
    float3 color         : COLOR;
};

struct VS_OUTPUT {    
    float4 position_sc      : SV_POSITION;
    float3 color            : COLOR;
};


cbuffer camera: register (b0) {
    float4x4 view_projection_matrix;
}



VS_OUTPUT main(VS_INPUT input) {   
    VS_OUTPUT output;
    output.position_sc = mul(float4(input.position, 1.0), view_projection_matrix);
    output.color = input.color;
    return output;
}