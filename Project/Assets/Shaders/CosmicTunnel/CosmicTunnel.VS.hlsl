// CosmicTunnel 頂点シェーダー
struct VSInput
{
    float4 position : POSITION;
    float2 texcoord : TEXCOORD0;
};

struct VSOutput
{
    float4 svPosition : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 worldPosition : TEXCOORD1;
};

struct TransformationMatrix
{
    float4x4 WVP;
    float4x4 World;
};

ConstantBuffer<TransformationMatrix> gTransform : register(b0);

VSOutput main(VSInput input)
{
    VSOutput output;
    
    // WVP行列で座標変換
    output.svPosition = mul(input.position, gTransform.WVP);
    
    // ワールド座標を計算
    output.worldPosition = mul(input.position, gTransform.World).xyz;
    
    // UVをそのまま渡す
    output.uv = input.texcoord;
    
    return output;
}
