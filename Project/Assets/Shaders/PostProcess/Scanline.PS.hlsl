#include "FullScreen.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

cbuffer ScanlineParams : register(b0)
{
    float lineIntensity;      // 線の強度
    float lineWidth;          // 線の幅
    float lineSpeed;          // 線のスクロール速度
    float lineFrequency;      // 線の周波数
    
    float flickerIntensity;   // フリッカー強度
    float flickerSpeed;       // フリッカー速度
    float time;               // 経過時間
    float padding;
}

struct PixelShaderInput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
};

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

// ノイズ関数
float hash(float n)
{
    return frac(sin(n) * 43758.5453);
}

PixelShaderOutput main(PixelShaderInput input)
{
    PixelShaderOutput output;
    
    // 元の色をサンプル
    float4 baseColor = gTexture.Sample(gSampler, input.texcoord);
    
    // 画面解像度（1080pを想定）
    float screenHeight = 1080.0;
    
    // Y座標をピクセル単位に変換
    float pixelY = input.texcoord.y * screenHeight;
    
    // 走査線の位置（スクロール）
    float scrollOffset = time * lineSpeed;
    float adjustedY = (pixelY + scrollOffset) * lineFrequency;
    
    // 走査線パターン（sin波で滑らかな線を生成）
    float scanlinePattern = sin(adjustedY * 3.14159 / lineWidth);
    scanlinePattern = (scanlinePattern + 1.0) * 0.5; // 0-1の範囲に正規化
    
    // 走査線の強度を計算
    float scanlineFactor = 1.0 - (scanlinePattern * lineIntensity);
    
    // フリッカー効果（画面全体の微妙な明滅）
    float flickerNoise = hash(floor(time * flickerSpeed));
    float flicker = 1.0 - (flickerNoise * flickerIntensity);
    
    // 走査線とフリッカーを適用
    float3 finalColor = baseColor.rgb * scanlineFactor * flicker;
    
    // 少しだけコントラストを上げる（CRTモニター風）
    finalColor = pow(finalColor, 0.95);
    
    output.color = float4(finalColor, baseColor.a);
    
    return output;
}
