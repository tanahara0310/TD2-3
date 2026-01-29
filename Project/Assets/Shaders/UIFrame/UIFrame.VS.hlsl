// UIFrame 頂点シェーダー（ビルボード対応）
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
    float4x4 ViewInverse;
    float4 cameraPosition;
};

ConstantBuffer<TransformationMatrix> gTransform : register(b0);

VSOutput main(VSInput input)
{
    VSOutput output;
    
    // ビルボード処理：カメラの向きに合わせて頂点を回転
    float3 right = gTransform.ViewInverse[0].xyz;
    float3 up = gTransform.ViewInverse[1].xyz;
    
    // オブジェクトの中心位置（ワールド座標）
    float3 centerPos = gTransform.World[3].xyz;
    
    // ビルボード頂点位置を計算（カメラに対して常に正面を向く）
    float3 billboardPos = centerPos 
        + right * input.position.x 
        + up * input.position.y;
    
    // ワールド座標を保存
    output.worldPosition = billboardPos;
    
    // 最終座標を計算
    float4 worldPos = float4(billboardPos, 1.0);
    output.svPosition = mul(worldPos, gTransform.WVP);
    
    // UVをそのまま渡す
    output.uv = input.texcoord;
    
    return output;
}
