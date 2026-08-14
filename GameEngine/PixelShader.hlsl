cbuffer constantBuffer
{
    float4 face_colors[6];
};

float4 main(uint triangleID : SV_PrimitiveID) : SV_TARGET
{
    return face_colors[triangleID / 2.0f];

}