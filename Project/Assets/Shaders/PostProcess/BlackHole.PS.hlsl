// テクスチャとサンプラー
Texture2D gTexture : register(t0);
SamplerState gSampler : register(s0);

// ブラックホールパラメータ用の定数バッファ
cbuffer BlackHoleParams : register(b0)
{
    float time;           // 時間パラメータ
    float intensity;      // エフェクト強度
    float speed;          // 回転速度
    float distortion;     // 歪み強度
};

struct PSInput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
};

struct PSOutput
{
    float4 color : SV_TARGET;
};

// ハッシュ関数（疑似乱数生成）
float2 Hash2(float2 p)
{
    p = float2(dot(p, float2(127.1, 311.7)),
               dot(p, float2(269.5, 183.3)));
    return frac(sin(p) * 43758.5453);
}

// 2x2回転行列
float2x2 Rotate(float angle)
{
    float c = cos(angle);
    float s = sin(angle);
    return float2x2(c, s, -s, c);
}

// Voronoiノイズ関数
float VoronoiNoise(float2 p)
{
    float d = 1000.0;
    
    float2 q = frac(p);
    float2 f = floor(p);
    
    for (float x = -0.5; x <= 0.5; x += 1.0)
    {
        for (float y = -0.5; y <= 0.5; y += 1.0)
        {
            float2 h = float2(x, y);
            // ハッシュ関数からランダム値を取得
            float2 randomOffset = Hash2(f + h);
            d = min(d, length(q - h - randomOffset));
        }
    }
    
    return d;
}

PSOutput main(PSInput input)
{
    PSOutput output;
    
    // テクスチャ座標を[-1, 1]の範囲に変換し、アスペクト比を考慮
    float2 uv = input.texcoord;
    float2 p = (uv * 2.0 - 1.0);
    
    // アスペクト比の調整（1280x720を想定）
    p.x *= 1280.0 / 720.0;
    
    // 中心の小さな円は黒くする
    if (length(p) < 0.1)
    {
        output.color = float4(0, 0, 0, 1);
        return output;
    }
    
    // 歪み効果：反転と極座標変換
    p /= dot(p, p) * distortion;
    
    // 回転効果
    float rotationAngle = pow(dot(p, p), 1.5) * 0.4;
    p = mul(Rotate(rotationAngle), p);
    
    // エフェクトカラーの計算
    float4 effectColor = float4(0, 0, 0, 0);
    float t = -time * speed;
    
    // 複数レイヤーのVoronoiノイズを重ねる
    for (float n = frac(t); n < 32.0; n += 1.0)
    {
        // ハッシュ関数からオフセットを取得
        float2 noiseOffset = Hash2(float2(floor(n - floor(t)), 0)) * 256.0;
        
        // Voronoiノイズの計算
        float voronoi = VoronoiNoise(p * n * 0.4 + noiseOffset);
        float contribution = pow(voronoi * 1000.0 + 10.0, -1.0);
        
        // フェードイン・アウト
        float fadeIn = saturate(n);
        float fadeOut = saturate(1.0 - n / 30.0);
        
        effectColor += contribution * fadeOut * fadeIn;
    }
    
    // ベースの明るさを追加
    effectColor += dot(p, p) * 0.037;
    
    // カラーグレーディング（紫のグラデーション）
    effectColor *= float4(13, 11, 18, 0);
    effectColor -= dot(p, p) * 0.6 * float4(0.9, 1.2, 1.4, 0);
    
    // 色を正規化
    effectColor = max(effectColor, 0.0);
    effectColor *= effectColor;
    
    // 元のテクスチャと合成
    float4 originalColor = gTexture.Sample(gSampler, uv);
    output.color = lerp(originalColor, effectColor, intensity);
    output.color.a = 1.0;
    
    return output;
}

