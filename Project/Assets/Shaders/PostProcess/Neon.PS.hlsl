#include "FullScreen.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

// ネオンパラメータ用の定数バッファ
cbuffer NeonParams : register(b0)
{
    float edgeThreshold;        // エッジ検出の閾値
    float glowIntensity;        // グローの強度
    float edgeWidth;            // エッジの太さ
    float colorSaturation;      // 色の彩度
    float brightness;           // 明るさ
    float neonColorR;           // ネオンカラーR
    float neonColorG;           // ネオンカラーG
    float neonColorB;           // ネオンカラーB
};

struct PixelShaderInput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
};

struct PixelShaderOutput
{
    float4 color : SV_Target;
};

// 輝度計算（Rec.709）
float Luminance(float3 color)
{
    return dot(color, float3(0.2126f, 0.7152f, 0.0722f));
}

// RGB to HSV変換
float3 RGBtoHSV(float3 rgb)
{
    float4 K = float4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    float4 p = lerp(float4(rgb.bg, K.wz), float4(rgb.gb, K.xy), step(rgb.b, rgb.g));
    float4 q = lerp(float4(p.xyw, rgb.r), float4(rgb.r, p.yzx), step(p.x, rgb.r));
    
    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return float3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

// HSV to RGB変換
float3 HSVtoRGB(float3 hsv)
{
    float4 K = float4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    float3 p = abs(frac(hsv.xxx + K.xyz) * 6.0 - K.www);
    return hsv.z * lerp(K.xxx, saturate(p - K.xxx), hsv.y);
}

// Sobelフィルタによるエッジ検出
float DetectEdges(float2 texcoord, float2 texelSize)
{
    // Sobelカーネル（X方向）
    float sobelX[9] = {
        -1.0, 0.0, 1.0,
        -2.0, 0.0, 2.0,
        -1.0, 0.0, 1.0
    };
    
    // Sobelカーネル（Y方向）
    float sobelY[9] = {
        -1.0, -2.0, -1.0,
         0.0,  0.0,  0.0,
         1.0,  2.0,  1.0
    };
    
    float edgeX = 0.0;
    float edgeY = 0.0;
    
    // 3x3カーネルを適用
    for (int y = -1; y <= 1; y++)
    {
        for (int x = -1; x <= 1; x++)
        {
            float2 offset = float2(x, y) * texelSize * edgeWidth;
            float4 color = gTexture.Sample(gSampler, texcoord + offset);
            float lum = Luminance(color.rgb);
            
            int index = (y + 1) * 3 + (x + 1);
            edgeX += lum * sobelX[index];
            edgeY += lum * sobelY[index];
        }
    }
    
    // エッジの強度を計算
    float edgeStrength = sqrt(edgeX * edgeX + edgeY * edgeY);
    
    return edgeStrength;
}

// グロー効果の計算
float4 ApplyGlow(float2 texcoord, float2 texelSize, float edgeStrength)
{
    float4 glowColor = float4(0, 0, 0, 0);
    float totalWeight = 0.0;
    
    // グローの範囲
    int glowRadius = 3;
    
    for (int y = -glowRadius; y <= glowRadius; y++)
    {
        for (int x = -glowRadius; x <= glowRadius; x++)
        {
            float2 offset = float2(x, y) * texelSize;
            float distance = length(float2(x, y));
            
            // ガウシアン重み
            float weight = exp(-distance * distance / (2.0 * glowRadius * glowRadius));
            
            float4 sampleColor = gTexture.Sample(gSampler, texcoord + offset);
            glowColor += sampleColor * weight;
            totalWeight += weight;
        }
    }
    
    return glowColor / totalWeight;
}

PixelShaderOutput main(PixelShaderInput input)
{
    PixelShaderOutput output;
    
    // テクセルサイズの計算
    float2 texelSize = 1.0 / float2(1280, 720);
    
    // 元の色を取得
    float4 originalColor = gTexture.Sample(gSampler, input.texcoord);
    
    // エッジを検出
    float edgeStrength = DetectEdges(input.texcoord, texelSize);
    
    // エッジの閾値処理
    float edgeMask = smoothstep(edgeThreshold - 0.1, edgeThreshold + 0.1, edgeStrength);
    
    // グロー効果を適用
    float4 glowColor = ApplyGlow(input.texcoord, texelSize, edgeStrength);
    
    // 彩度を調整
    float3 hsv = RGBtoHSV(originalColor.rgb);
    hsv.y *= colorSaturation;
    float3 saturatedColor = HSVtoRGB(hsv);
    
    // ネオンカラーを作成
    float3 neonColor = float3(neonColorR, neonColorG, neonColorB);
    
    // エッジにネオンカラーを適用
    float3 edgeColor = neonColor * edgeMask * glowIntensity;
    
    // グローとエッジを合成
    float3 glowEffect = glowColor.rgb * edgeMask * glowIntensity * neonColor;
    
    // 最終的な色を計算
    float3 finalColor = saturatedColor * brightness + edgeColor + glowEffect;
    
    // 暗い部分をより暗くする（ネオン効果を強調）
    float darkMask = 1.0 - Luminance(originalColor.rgb);
    finalColor = lerp(finalColor, finalColor * darkMask, 0.3);
    
    output.color = float4(finalColor, originalColor.a);
    
    return output;
}
