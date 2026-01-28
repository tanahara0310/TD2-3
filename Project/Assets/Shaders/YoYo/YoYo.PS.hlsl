#include "YoYo.hlsli"

struct YoYoMaterial
{
    float4 color;
    int enableLighting;
    float time;
    float sparkIntensity;
    float padding;
};

struct Camera
{
    float3 worldPosition;
};

cbuffer MaterialBuffer : register(b0)
{
    YoYoMaterial gMaterial;
};

cbuffer CameraBuffer : register(b2)
{
    Camera gCamera;
};

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

// Cosmic palette function (from "Cosmic" shader by @XorDev)
float3 palette(float t)
{
    float3 a = float3(0.731, 1.098, 0.192);
    float3 b = float3(0.358, 1.090, 0.657);
    float3 c = float3(1.077, 0.360, 0.328);
    float3 d = float3(0.965, 2.265, 0.837);
    
    return a + b * cos(6.2832 * (c * t + d));
}

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    // 極座標系のUV座標を受け取る
    // input.texcoord.x = 半径（0-1）
    // input.texcoord.y = 角度（0-1）
    
    float radius = input.texcoord.x;  // 0（中心）～ 1（外縁）
    float angleNormalized = input.texcoord.y;  // 0 ～ 1
    
    // 中心からの位置をスクリーン座標風に変換
    float2 uv;
    uv.x = radius * cos(angleNormalized * 6.2832); // 2π
    uv.y = radius * sin(angleNormalized * 6.2832);
    
    // スケールを調整
    uv *= 256.0;
    
    float4 fragColor = float4(0.0, 0.0, 0.0, 0.0);
    
    // 同心円リングを描画（30レイヤー）
    for (float index = 0.0; index < 30.0; index += 1.0)
    {
        // 距離の計算
        float dist = length(uv);
        
        // リングを波打たせる（伸び縮みさせる）
        float ringPhase = index * 0.5; // 各リングに位相差を持たせる
        float ringPulse = sin(gMaterial.time * 3.0 + ringPhase) * 0.3 + 1.0; // 0.7 ~ 1.3の範囲で変化
        float ringRadius = index * 8.0 * ringPulse;
        
        // リングの幅（細くしてシャープに）
        float w = abs(dist - ringRadius) + 0.5;
        
        // 角度計算（時間による回転を除去）
        float arc = atan2(uv.y, uv.x) * ceil(index * 0.1) + index * index;
        
        // リングの明るさ（より広い範囲で光らせる）
        float brightness = clamp(cos(arc), 0.0, 0.8);
        brightness = pow(brightness, 0.7); // コントラストを上げる
        
        // 色を計算（時間で色相を大きく変化させる）
        float colorShift = gMaterial.time * 2.0 + index * 0.5;
        
        // 色相環を作成（より鮮やかな虹色）
        float hue = frac(colorShift / 6.2832);
        float3 rainbow;
        rainbow.r = abs(hue * 6.0 - 3.0) - 1.0;
        rainbow.g = 2.0 - abs(hue * 6.0 - 2.0);
        rainbow.b = 2.0 - abs(hue * 6.0 - 4.0);
        rainbow = saturate(rainbow);
        
        // さらに波の効果を追加
        float wave = cos(arc - colorShift) * 0.5 + 0.5;
        
        float4 color = float4(lerp(rainbow, rainbow * 1.5, wave), 1.0);
        
        // より強く光らせる
        fragColor += 0.3 / w * brightness * color;
    }
    
    // 基本色と合成（より明るく）
    float3 finalColor = fragColor.rgb * gMaterial.color.rgb * 3.0;
    
    // 火花エフェクトを追加（ヨーヨーの外側のみ）
    if (gMaterial.sparkIntensity > 0.0 && radius > 0.8)
    {
        // 極座標から火花パターンを計算
        float sparkAngle = atan2(uv.y, uv.x);
        
        // ヨーヨーの縁からの距離（0.8を基準に外側）
        float distFromEdge = (radius - 0.8) / 0.2; // 0.8～1.0を0～1にマッピング
        distFromEdge = clamp(distFromEdge, 0.0, 1.0);
        
        // 火花を外側に散らす（1.0より外も含める）
        float sparkRadius = radius - 0.8; // 縁からの距離
        
        // 放射状の火花（32本）
        float sparkCount = 32.0;
        float sparkLine = frac(sparkAngle / (3.14159 * 2.0) * sparkCount);
        
        // より鋭い火花
        float spark = smoothstep(0.47, 0.5, sparkLine) - smoothstep(0.5, 0.53, sparkLine);
        spark = pow(spark, 0.5); // 明るさを強調
        
        // 時間による激しい明滅
        float sparkFlicker = sin(gMaterial.time * 20.0 + sparkAngle * 15.0) * 0.5 + 0.5;
        sparkFlicker = pow(sparkFlicker, 0.3); // コントラストを上げる
        
        // 外側に向かってフェードアウト（0.8から外へ）
        float sparkFade = 1.0 - smoothstep(0.0, 3.0, sparkRadius); // 3.0まで広がる
        sparkFade = pow(sparkFade, 0.5); // フェードカーブを調整
        
        // 縁から出る効果
        float edgeGlow = 1.0 - distFromEdge; // 縁で最大
        edgeGlow = pow(edgeGlow, 0.3);
        
        // 火花の色（オレンジ～白）
        float3 sparkColorHot = float3(1.0, 1.0, 0.9);  // ほぼ白（超高温）
        float3 sparkColorWarm = float3(1.0, 0.6, 0.1); // オレンジ（高温）
        float3 sparkColor = lerp(sparkColorWarm, sparkColorHot, sparkFlicker);
        
        // 二次的な火花（細かい粒子）- 外側のみ
        float sparkNoise = frac(sin(sparkAngle * 50.0 + gMaterial.time * 10.0) * 43758.5453);
        float sparkParticles = sparkNoise > 0.7 ? 1.0 : 0.0;
        
        // 火花を強く加算（縁から外側のみ）
        float sparkIntensity = spark * sparkFade * sparkFlicker * edgeGlow * gMaterial.sparkIntensity;
        finalColor += sparkColor * sparkIntensity * 20.0; // 20倍に強化
        
        // パーティクルを追加（外側に散る）
        float particleIntensity = sparkParticles * sparkFade * edgeGlow * gMaterial.sparkIntensity;
        finalColor += sparkColor * particleIntensity * 8.0;
    }
    
    // ライティングを適用する場合
    if (gMaterial.enableLighting != 0)
    {
        float3 lightDir = normalize(float3(0.5, -1.0, 0.5));
        float3 normal = normalize(input.normal);
        
        // ディフューズ
        float diffuse = max(dot(normal, -lightDir), 0.0);
        
        // アンビエント（明るくする）
        float ambient = 0.6;
        
        // スペキュラ（強めに）
        float3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
        float3 halfVector = normalize(-lightDir + toEye);
        float specular = pow(max(dot(normal, halfVector), 0.0), 15.0);
        
        // ライティング適用（エフェクトの明るさを保つ）
        finalColor = finalColor * (ambient + diffuse * 0.15) + float3(1.0, 1.0, 1.0) * specular * 0.6;
    }
    
    output.color = float4(finalColor, gMaterial.color.a);
    
    return output;
}
