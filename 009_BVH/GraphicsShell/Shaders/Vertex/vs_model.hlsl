struct VS_INPUT {
    float3 position      : POSITION;
    float3 normal        : NORMAL;    
    float2 uv            : TEXCOORD;
};

struct VS_OUTPUT {
    
    float4 position_sc      : SV_POSITION;
    float3 normal           : NORMAL;    
    float2 uv               : TEXCOORD;
    uint   instanceId       : INSTANCE_ID;
};


cbuffer camera: register (b0) {
    float4x4 view_projection_matrix;
}

cbuffer Offset: register(b1) {
    int modelMatrixBufferOffset;
}

cbuffer model: register(b2) {
    float4x4 modelMatrix[1000];
}





VS_OUTPUT main(VS_INPUT input, uint  instanceid : SV_InstanceID) {   
    VS_OUTPUT output;
    float4x4 model_matrix = modelMatrix[modelMatrixBufferOffset + instanceid];
    float4 pos_wld = mul(float4(input.position, 1.0), model_matrix);
    output.position_sc = mul(pos_wld, view_projection_matrix) ;
    output.uv = input.uv;
    output.instanceId = instanceid;
    output.normal = input.normal;
    return output;
}