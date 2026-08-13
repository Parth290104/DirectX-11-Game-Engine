struct VertexShaderOutput
{
    float3 color : COLOR;
    float4 pos : SV_Position;
};

VertexShaderOutput main( float2 pos : POSITION, float3 color : COLOR )
{
    VertexShaderOutput vertexShaderOutput;
    vertexShaderOutput.pos = float4(pos, 0.0f, 1.0f);
    vertexShaderOutput.color = color;
    return vertexShaderOutput;
}