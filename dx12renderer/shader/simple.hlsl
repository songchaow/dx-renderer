/* struct Vertex {
    XMFLOAT3 POS;
    // ...
}; */




void VS(float3 posWorld : POSITION, out float4 posNDC : SV_POSITION) {
    posNDC = float4(posWorld, 1.0);
    
}