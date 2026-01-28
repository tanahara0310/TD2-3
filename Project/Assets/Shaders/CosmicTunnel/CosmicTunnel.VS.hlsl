// CosmicTunnel 頂点シェーダー（ビルボード対応）
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
    float4x4 ViewInverse; // ビュー行列の逆行列
    float4 cameraPosition; // カメラ位置
};

ConstantBuffer<TransformationMatrix> gTransform : register(b0);

VSOutput main(VSInput input)
{
    VSOutput output;
    
    // ビルボード処理：カメラの向きに合わせて頂点を回転
    // ビュー逆行列から右、上、前ベクトルを取得
    float3 right = gTransform.ViewInverse[0].xyz;
    float3 up = gTransform.ViewInverse[1].xyz;
    float3 forward = gTransform.ViewInverse[2].xyz;
    
    // オブジェクトの中心位置（ワールド座標）
    float3 centerPos = gTransform.World[3].xyz;
    
    // ビルボード頂点位置を計算（カメラに対して常に正面を向く）
    float3 billboardPos = centerPos 
        + right * input.position.x 
        + up * input.position.y;
    
    // ワールド座標を保存
    output.worldPosition = billboardPos;
    
    // ビュープロジェクション行列で最終座標を計算
    float4 worldPos = float4(billboardPos, 1.0);
    
    // WVP行列を使わず、ビューとプロジェクションを分けて適用
    // ここでは簡易的にWVPを使用（後でビュー行列を渡すように変更）
    float4x4 viewProj = gTransform.WVP;
    // ワールド行列の影響を除去するため、centerPosを基準に再計算
    output.svPosition = mul(worldPos, viewProj);
    
    // UVをそのまま渡す
    output.uv = input.texcoord;
    
    return output;
}
