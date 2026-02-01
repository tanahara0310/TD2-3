#include "FullScreen.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

cbuffer RetroGridParams : register(b0)
{
    float time;
    float gridSize;
    float lineWidth;
    float scanlineSpeed;
    float scanlineIntensity;
    float gridColorR;
    float gridColorG;
    float gridColorB;
    float gridAlpha;
    float horizonGlow;
    float perspectiveStrength;
    float noiseIntensity;
    float padding;
}

struct PixelShaderInput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
};

struct PixelShaderOutput
{
    float4 color : SV_Target;
};

// 簡易ハッシュ関数
float hash(float2 p)
{
    return frac(sin(dot(p, float2(127.1, 311.7))) * 43758.5453);
}

// ノイズ関数
float noise(float2 p)
{
    float2 i = floor(p);
    float2 f = frac(p);
    f = f * f * (3.0 - 2.0 * f);
    
    float a = hash(i);
    float b = hash(i + float2(1.0, 0.0));
    float c = hash(i + float2(0.0, 1.0));
    float d = hash(i + float2(1.0, 1.0));
    
    return lerp(lerp(a, b, f.x), lerp(c, d, f.x), f.y);
}

PixelShaderOutput main(PixelShaderInput input)
{
    PixelShaderOutput output;
    
    // 元のテクスチャを取得
    float4 originalColor = gTexture.Sample(gSampler, input.texcoord);
    
    // UV座標を変換（0,0が中心になるように）
    float2 uv = input.texcoord * 2.0 - 1.0;
    uv.y = -uv.y; // Y軸を反転
    
    // パースペクティブ効果（下に行くほど遠くなる）
    float perspective = lerp(1.0, 0.3, (1.0 - input.texcoord.y) * perspectiveStrength);
    
    // グリッド座標を計算
    float2 gridUV = float2(uv.x, uv.y + time * 0.2) * gridSize * perspective;
    
    // グリッドラインを計算
    float2 grid = abs(frac(gridUV) - 0.5);
    float gridLine = 1.0 - smoothstep(lineWidth, lineWidth + 0.01, min(grid.x, grid.y));
    
    // 地平線グロー（下部をより明るく）
    float horizonFade = pow(1.0 - input.texcoord.y, 2.0) * horizonGlow;
    
    // スキャンライン効果
    float scanline = sin(input.texcoord.y * 200.0 + time * scanlineSpeed * 10.0) * 0.5 + 0.5;
    scanline = scanline * scanlineIntensity;
    
    // ノイズ効果（レトロな感じを出す）
    float noiseValue = noise(input.texcoord * 100.0 + time) * noiseIntensity;
    
    // グリッドカラーを作成
    float3 gridColor = float3(gridColorR, gridColorG, gridColorB);
    
    // グリッドの明るさを計算（パースと地平線グローを適用）
    float gridBrightness = gridLine * perspective + horizonFade;
    
    // 最終的なグリッド色
    float3 finalGridColor = gridColor * gridBrightness;
    
    // スキャンラインとノイズを追加
    finalGridColor += scanline + noiseValue;
    
    // 元の色とグリッドをブレンド（アルファブレンディング）
    output.color.rgb = lerp(originalColor.rgb, finalGridColor, gridAlpha * gridLine);
    
    // 地平線グローを加算
    output.color.rgb += gridColor * horizonFade * 0.3;
    
    output.color.a = originalColor.a;
    
    return output;
}
