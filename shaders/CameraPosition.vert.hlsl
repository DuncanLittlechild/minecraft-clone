cbuffer MvpTrans : register(b0, space1)
{
    row_major float4x4 MoveTransform;
};
cbuffer VpTrans : register(b1, space1)
{
    row_major float4x4 VpTransform;
};

struct Input
{
    float3 position : TEXCOORD0;
    float3 norms : TEXCOORD1;
    float2 texStart : TEXCOORD2;
    float2 texEnd : TEXCOORD3;
};

struct Output
{
    float4 Position : SV_Position;
    float3 norms : TEXCOORD1;
    float2 texStart : TEXCOORD2;
    float2 texEnd : TEXCOORD3;
};

Output main (Input input)
{
    Output output;
    output.Position = mul(float4(input.position, 1.0f), mul(MoveTransform, VpTransform));
    output.norms = input.norms;
    output.texStart = input.texStart;
    output.texEnd = input.texEnd;
    return output;
}