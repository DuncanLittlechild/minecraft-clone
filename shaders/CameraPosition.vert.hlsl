cbuffer VpTrans : register(b0, space1)
{
    row_major float4x4 VpTransform;
};

struct Input
{
    float3 position : TEXCOORD0;
    float3 norms : TEXCOORD1;
    float2 textureCoords : TEXCOORD2;
    int tIndex : TEXCOORD3;
};

struct Output
{
    float2 textureCoords : TEXCOORD0;
    nointerpolation int tIndex : TEXCOORD1;
    float4 Position : SV_Position;
};

Output main (Input input)
{
    Output output;
    output.Position = mul(float4(input.position, 1.0f), VpTransform);
    output.textureCoords = input.textureCoords;
    output.tIndex = input.tIndex;
    return output;
}