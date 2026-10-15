cbuffer MvpTrans : register(b0, space1)
{
    row_major float4x4 MoveTransform;
};
cbuffer VpTrans : register(b1, space1)
{
    row_major float4x4 VpTransform;
};
cbuffer TexIndex : register(b2, space1)
{
    float TextureIndex;
};

struct Input
{
    float3 position : TEXCOORD0;
    float3 norms : TEXCOORD1;
    float2 textureCoords : TEXCOORD2;
};

struct Output
{
    float2 textureCoords : TEXCOORD0;
    float tIndex : TEXCOORD1;

    float3 norms : TEXCOORD2;

    float4 Position : SV_Position;
};

Output main (Input input)
{
    Output output;
    output.Position = mul(float4(input.position, 1.0f), mul(MoveTransform, VpTransform));
    output.norms = input.norms;
    output.textureCoords = input.textureCoords;
    output.tIndex = TextureIndex;
    return output;
}