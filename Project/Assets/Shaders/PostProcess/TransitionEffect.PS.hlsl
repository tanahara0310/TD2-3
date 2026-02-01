#include "FullScreen.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

cbuffer TransitionParams : register(b0)
{
    // float4境界 (16bytes) #1
    float progress;             // トランジション進行度 (0.0-1.0)
    int transitionType;         // トランジションタイプ
    float centerX;              // 中心X座標
    float centerY;              // 中心Y座標
    
    // float4境界 (16bytes) #2
    float smoothness;           // スムーズネス
    float slideAngle;           // スライド角度
    float glitchIntensity;      // グリッチ強度
    float pixelSize;            // ピクセルサイズ
    
    // float4境界 (16bytes) #3
    float aspectRatio;          // アスペクト比
    float3 maskColor;           // マスク色RGB
    
    // float4境界 (16bytes) #4
    float blindCount;           // ブラインド数
    float randomSeed;           // ランダムシード
    float waveFrequency;        // 波の周波数
    float waveAmplitude;        // 波の振幅
}

static const float PI = 3.14159265359;

// ランダム関数
float random(float2 st)
{
    return frac(sin(dot(st.xy, float2(12.9898, 78.233)) + randomSeed) * 43758.5453123);
}

// ノイズ関数
float noise(float2 st)
{
    float2 i = floor(st);
    float2 f = frac(st);
    
    float a = random(i);
    float b = random(i + float2(1.0, 0.0));
    float c = random(i + float2(0.0, 1.0));
    float d = random(i + float2(1.0, 1.0));
    
    float2 u = f * f * (3.0 - 2.0 * f);
    return lerp(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

// Circle Transition (円形トランジション)
float CircleTransition(float2 uv)
{
    float2 centeredUV = uv - float2(centerX, centerY);
    centeredUV.x *= aspectRatio;
    
    float distanceFromCenter = length(centeredUV);
    float maxRadius = length(float2(centerX * aspectRatio, centerY)) + 
                      length(float2((1.0 - centerX) * aspectRatio, 1.0 - centerY));
    
    float currentRadius = maxRadius * (1.0 - progress);
    return smoothstep(currentRadius - smoothness, currentRadius + smoothness, distanceFromCenter);
}

// Slide Transition (スライドトランジション)
float SlideTransition(float2 uv)
{
    float angle = radians(slideAngle);
    float2 direction = float2(cos(angle), sin(angle));
    
    float projectedPos = dot(uv - float2(0.5, 0.5), direction) + 0.5;
    // progress=0で-0.25、progress=1で1.25の範囲をカバー
    float slidePos = progress * 1.5 - 0.25;
    
    // projectedPosがslidePosより小さい場合に1（黒）、大きい場合に0（元の色）
    return smoothstep(slidePos + smoothness, slidePos - smoothness, projectedPos);
}

// Glitch Transition (グリッチトランジション)
float4 GlitchTransition(float2 uv, float4 originalColor)
{
    // progress=0のときは元の色をそのまま返す
    if (progress < 0.01)
    {
        return originalColor;
    }
    
    // RGB分離の強度をprogressに応じて調整
    float glitchStrength = progress * glitchIntensity;
    float offsetR = (noise(float2(uv.y * 10.0, progress * 50.0)) - 0.5) * glitchStrength * 0.05;
    float offsetG = (noise(float2(uv.y * 12.0, progress * 48.0)) - 0.5) * glitchStrength * 0.05;
    float offsetB = (noise(float2(uv.y * 11.0, progress * 52.0)) - 0.5) * glitchStrength * 0.05;
    
    float4 colorR = gTexture.Sample(gSampler, uv + float2(offsetR, 0.0));
    float4 colorG = gTexture.Sample(gSampler, uv + float2(offsetG, 0.0));
    float4 colorB = gTexture.Sample(gSampler, uv + float2(offsetB, 0.0));
    
    float4 glitchedColor = float4(colorR.r, colorG.g, colorB.b, 1.0);
    
    // ブロックノイズ（progressが進むにつれて強くなる）
    float blockNoise = noise(float2(floor(uv.y * 20.0), progress * 10.0));
    if (blockNoise > (1.0 - progress * 0.5) && progress > 0.2)
    {
        float shift = (noise(float2(uv.y * 5.0, progress * 20.0)) - 0.5) * glitchStrength * 0.2;
        glitchedColor = gTexture.Sample(gSampler, uv + float2(shift, 0.0));
    }
    
    // フェード（0.6から徐々に黒くなる）
    float fade = smoothstep(0.6, 1.0, progress);
    glitchedColor.rgb = lerp(glitchedColor.rgb, maskColor, fade);
    
    return glitchedColor;
}

// Pixelate Transition (ピクセル分解トランジション)
float PixelateTransition(float2 uv)
{
    float currentPixelSize = lerp(1.0, pixelSize, progress);
    float2 pixelUV = floor(uv * (1920.0 / currentPixelSize)) / (1920.0 / currentPixelSize);
    
    float randValue = random(pixelUV);
    // randValueがprogressより小さいピクセルから徐々に黒くなる
    return smoothstep(progress + 0.1, progress - 0.1, randValue);
}

// Wipe Transition (ワイプトランジション)
float WipeTransition(float2 uv)
{
    float wave = sin(uv.y * waveFrequency * PI) * waveAmplitude;
    // progress=0で-0.1、progress=1で1.1の範囲
    float wipePos = progress * 1.2 - 0.1 + wave;
    
    // uv.xがwipePosより小さい場合に1（黒）、大きい場合に0（元の色）
    return smoothstep(wipePos + smoothness, wipePos - smoothness, uv.x);
}

// Blind Transition (ブラインドトランジション)
float BlindTransition(float2 uv)
{
    float blindIndex = floor(uv.y * blindCount);
    float blindPos = frac(uv.y * blindCount);
    
    // 各ブラインドにランダムなオフセットを追加
    float offset = random(float2(blindIndex, 0.0)) * 0.2;
    float adjustedProgress = progress * 1.2 - 0.1 + offset;
    
    // blindPosがadjustedProgressより小さい場合に1（黒）
    return smoothstep(adjustedProgress + smoothness, adjustedProgress - smoothness, blindPos);
}

// ZoomBlur Transition (ズームブラートランジション)
float4 ZoomBlurTransition(float2 uv, float4 originalColor)
{
    // progress=0のときは元の色をそのまま返す
    if (progress < 0.01)
    {
        return originalColor;
    }
    
    float2 center = float2(centerX, centerY);
    float2 direction = uv - center;
    
    float4 color = float4(0, 0, 0, 0);
    int samples = 10;
    
    // progressに応じてブラーの強度を調整
    for (int i = 0; i < samples; i++)
    {
        float t = float(i) / float(samples);
        float scale = 1.0 - progress * t * 0.5;
        float2 sampleUV = center + direction * scale;
        color += gTexture.Sample(gSampler, sampleUV);
    }
    
    color /= float(samples);
    
    // フェード（0.6から徐々に黒くなる）
    float fade = smoothstep(0.6, 1.0, progress);
    color.rgb = lerp(color.rgb, maskColor, fade);
    
    return color;
}

// Mosaic Transition (モザイクトランジション)
float4 MosaicTransition(float2 uv, float4 originalColor)
{
    // progress=0のときは元の色をそのまま返す
    if (progress < 0.01)
    {
        return originalColor;
    }
    
    // progressに応じてピクセルサイズが大きくなる
    float currentPixelSize = lerp(1.0, pixelSize, progress);
    float2 pixelUV = floor(uv * (1920.0 / currentPixelSize)) / (1920.0 / currentPixelSize);
    
    float4 pixelatedColor = gTexture.Sample(gSampler, pixelUV + float2(0.5 / (1920.0 / currentPixelSize), 0.5 / (1080.0 / currentPixelSize)));
    
    // フェード（0.7から徐々に黒くなる）
    float fade = smoothstep(0.7, 1.0, progress);
    pixelatedColor.rgb = lerp(pixelatedColor.rgb, maskColor, fade);
    
    return pixelatedColor;
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

PixelShaderOutput main(PixelShaderInput input)
{
    PixelShaderOutput output;
    float4 originalColor = gTexture.Sample(gSampler, input.texcoord);
    
    float mixFactor = 0.0;
    float4 transitionColor = originalColor;
    
    // トランジションタイプに応じて処理を分岐
    if (transitionType == 0) // Circle
    {
        mixFactor = CircleTransition(input.texcoord);
    }
    else if (transitionType == 1) // Slide
    {
        mixFactor = SlideTransition(input.texcoord);
    }
    else if (transitionType == 2) // Glitch
    {
        transitionColor = GlitchTransition(input.texcoord, originalColor);
        output.color = transitionColor;
        return output;
    }
    else if (transitionType == 3) // Pixelate
    {
        mixFactor = PixelateTransition(input.texcoord);
    }
    else if (transitionType == 4) // Wipe
    {
        mixFactor = WipeTransition(input.texcoord);
    }
    else if (transitionType == 5) // Blind
    {
        mixFactor = BlindTransition(input.texcoord);
    }
    else if (transitionType == 6) // ZoomBlur
    {
        transitionColor = ZoomBlurTransition(input.texcoord, originalColor);
        output.color = transitionColor;
        return output;
    }
    else if (transitionType == 7) // Mosaic
    {
        transitionColor = MosaicTransition(input.texcoord, originalColor);
        output.color = transitionColor;
        return output;
    }
    
    // 標準的なミックス処理
    output.color.rgb = lerp(originalColor.rgb, maskColor, mixFactor);
    output.color.a = 1.0;
    
    return output;
}
