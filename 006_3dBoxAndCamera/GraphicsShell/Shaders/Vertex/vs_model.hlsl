struct VS_INPUT {
    float3 position      : POSITION;
    float2 uv            : TEXCOORD;
};

struct VS_OUTPUT {
    float4 position         : SV_POSITION;
    float2 uv               : TEXCOORD;
    uint   instanceId       : INSTANCE_ID;
};


cbuffer camera: register (b0) {
    float4x4 view_projection_matrix;
}

cbuffer model: register(b1) {
    float4x4 model_matrix;
}



VS_OUTPUT main(VS_INPUT input, uint  instanceid : SV_InstanceID) {   
    VS_OUTPUT output;
    output.position = mul(float4(input.position, 1.0), view_projection_matrix) ;
    output.uv = input.uv;
    output.instanceId = instanceid;
    return output;
}