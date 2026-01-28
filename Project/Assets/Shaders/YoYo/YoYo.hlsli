// YoYo Shader Common Definitions

struct TransformationMatrix
{
    float4x4 WVP;
    float4x4 World;
    float4x4 WorldInversTranspose;
};

struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float3 worldPosition : WORLD_POSITION;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD0;
};
