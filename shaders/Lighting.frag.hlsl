Texture2DArray<float4> TextureArray : register(t0, space2);
SamplerState Sampler : register(s0, space2);

struct Input
{
    float2 textureCoords : TEXCOORD0;
    float tIndex : TEXCOORD1;
    float3 norms : TEXCOORD2;

    //float tIndex : TEXCOORD2;
};

float4 main (Input input) : SV_Target0
{

    return TextureArray.Sample(Sampler, float3(input.textureCoords, input.tIndex));
    //return float4(input.tIndex, 0, 0, 1);a
    //return TextureArray.Sample(Sampler, float3(input.textureCoords, 0));

}