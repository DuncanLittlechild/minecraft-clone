Texture2D<float> TextureAtlas : register(t0, space2);
SamplerState Sampler : register(s0, space2);

struct Input
{
    float4 Position : SV_Position;
    float3 norms : TEXCOORD1;
    float2 texStart : TEXCOORD2;
    float2 texEnd : TEXCOORD3;
};

float4 main (Input input) : SV_Target0
{
    return float4(1.0f, 0.0f, 0.0f, 1.0f);
}