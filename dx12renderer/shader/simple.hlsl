/* struct Vertex {
    XMFLOAT3 POS;
    // ...
}; */

cbuffer cbPerFrame : register(b0) {
    float4x4 world2cam;
    float4x4 cam2ndc;
}

cbuffer cbPerObject : register(b1) {
    float4x4 obj2world;
}

struct VertexIn {
    float3 posLocal : POSITION;

};

struct VertexOut {
    float4 posNDC : SV_POSITION;
};


VertexOut VS(VertexIn vin) {
    VertexOut ret;
    ret.posNDC = float4(vin.posLocal, 1.0);
    return ret;
}

float4 PS(VertexOut vout) : SV_TARGET {
    return float4(1,0,0,1);
}