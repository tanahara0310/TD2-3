#include "YoYo.hlsli"

cbuffer TransformationBuffer : register(b0)
{
    TransformationMatrix gTransformationMatrix;
};

struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    
    // プロシージャルにヨーヨーの形状を生成
    float u = input.texcoord.x; // 0 to 1 (円周方向)
    float v = input.texcoord.y; // 0 to 1 (高さ方向)
    
    // ヨーヨーのパラメータ
    float diskRadius = 1.0f;       // ディスクの半径
    float diskThickness = 0.15f;   // ディスクの厚さ
    float axleRadius = 0.15f;      // 軸の半径
    float axleHalfLength = 0.2f;   // 軸の半分の長さ
    
    // 角度（円周方向）
    float theta = u * 2.0f * 3.14159265f;
    
    // 高さをマッピング (-1 to 1)
    float height = (v - 0.5f) * 2.0f;
    
    float3 yoyoPos;
    float3 yoyoNormal;
    float2 effectUV; // エフェクト用のUV座標
    
    // 3つのセクションに分割：上のディスク、中央の軸、下のディスク
    if (height > 0.3f)
    {
        // 上のディスク
        float diskHeight = axleHalfLength;
        float diskV = (height - 0.3f) / 0.7f; // 0 to 1
        float currentRadius = lerp(axleRadius, diskRadius, diskV);
        float currentY = diskHeight + diskV * diskThickness;
        
        yoyoPos.x = currentRadius * cos(theta);
        yoyoPos.y = currentY;
        yoyoPos.z = currentRadius * sin(theta);
        
        // 法線（ディスクの傾斜に沿って）
        float3 radialDir = normalize(float3(cos(theta), 0.0f, sin(theta)));
        yoyoNormal = normalize(float3(radialDir.x, 0.5f, radialDir.z));
        
        // エフェクト用UV：極座標系（中心から外に向かって0-1、角度で一周）
        float normalizedRadius = currentRadius / diskRadius;
        effectUV.x = normalizedRadius;  // 半径方向（0 = 中心、1 = 外縁）
        effectUV.y = theta / (2.0f * 3.14159265f);  // 角度方向（0-1で一周）
    }
    else if (height < -0.3f)
    {
        // 下のディスク
        float diskHeight = -axleHalfLength;
        float diskV = (abs(height) - 0.3f) / 0.7f; // 0 to 1
        float currentRadius = lerp(axleRadius, diskRadius, diskV);
        float currentY = diskHeight - diskV * diskThickness;
        
        yoyoPos.x = currentRadius * cos(theta);
        yoyoPos.y = currentY;
        yoyoPos.z = currentRadius * sin(theta);
        
        // 法線（ディスクの傾斜に沿って）
        float3 radialDir = normalize(float3(cos(theta), 0.0f, sin(theta)));
        yoyoNormal = normalize(float3(radialDir.x, -0.5f, radialDir.z));
        
        // エフェクト用UV：極座標系（下のディスクも同じマッピング）
        float normalizedRadius = currentRadius / diskRadius;
        effectUV.x = normalizedRadius;  // 半径方向（0 = 中心、1 = 外縁）
        effectUV.y = theta / (2.0f * 3.14159265f);  // 角度方向（0-1で一周）
    }
    else
    {
        // 中央の軸部分（円柱）
        float axleV = (height + 0.3f) / 0.6f; // 0 to 1
        float currentY = lerp(-axleHalfLength, axleHalfLength, axleV);
        
        yoyoPos.x = axleRadius * cos(theta);
        yoyoPos.y = currentY;
        yoyoPos.z = axleRadius * sin(theta);
        
        // 円柱の法線（外向き）
        yoyoNormal = normalize(float3(cos(theta), 0.0f, sin(theta)));
        
        // エフェクト用UV：円柱も極座標で統一
        effectUV.x = 0.5f;  // 軸部分は半分の半径
        effectUV.y = theta / (2.0f * 3.14159265f);  // 角度方向（0-1で一周）
    }
    
    // ワールド座標に変換
    output.worldPosition = mul(float4(yoyoPos, 1.0f), gTransformationMatrix.World).xyz;
    output.position = mul(float4(yoyoPos, 1.0f), gTransformationMatrix.WVP);
    output.normal = normalize(mul(yoyoNormal, (float3x3)gTransformationMatrix.WorldInversTranspose));
    output.texcoord = effectUV; // エフェクト用のUV座標を使用
    
    return output;
}

