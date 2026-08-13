cbuffer ConstantBuffer
{
    row_major matrix transform;
};

struct VertexShaderOutput
{
    float3 color : COLOR;
    float4 pos : SV_Position;
};

VertexShaderOutput main( float2 pos : POSITION, float3 color : COLOR )
{
    VertexShaderOutput vertexShaderOutput;
    vertexShaderOutput.pos = mul(float4(pos.x, pos.y, 0.0f, 1.0f), transform);
    vertexShaderOutput.color = color;
    return vertexShaderOutput;
}