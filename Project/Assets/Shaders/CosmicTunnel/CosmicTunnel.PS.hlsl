// CosmicTunnel ピクセルシェーダー (レイマーチング)
struct PSInput
{
    float4 svPosition : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 worldPosition : TEXCOORD1;
};

struct SceneData
{
    float time;
    float3 cameraPosition;
    float2 resolution;
    float intensity;
    float padding;
};

ConstantBuffer<SceneData> gScene : register(b1);

// カスタムtanh関数（色の飽和処理）
float tanh_custom(float x)
{
    float e2x = exp(2.0 * x);
    return (e2x - 1.0) / (e2x + 1.0);
}

float3 tanh3(float3 x)
{
    return float3(tanh_custom(x.x), tanh_custom(x.y), tanh_custom(x.z));
}

// より鮮やかな飽和処理
float3 saturateColor(float3 col)
{
    return 1.0 - exp(-col * 0.8);
}

// オーブ距離関数 (Z, c)
float Orb(float3 p, float T, float Z, float c)
{
    float3 orbCenter = float3(
        10.0 + sin(0.4 * T + c * 2.0) * 16.0,
        sin(0.5 * T + c * 4.0) * 16.0,
        T * 4.0 + Z + 20.0 + 10.0 * cos(T * 0.6)
    );
    
    return length(p - orbCenter) - c;
}

// レイマーチングメイン関数
float4 main(PSInput input) : SV_TARGET
{
    float T = gScene.time;
    float2 u = input.uv * 2.0 - 1.0; // -1 to 1
    u.x *= gScene.resolution.x / gScene.resolution.y; // アスペクト比補正
    
    // 初期化
    float i = 0.0;
    float e, m, d = 0.0, s;
    bool b;
    float3 c = float3(0, 0, 0);
    float3 r = float3(gScene.resolution, 0);
    
    // レイの開始位置と方向
    float3 p = float3(10.0, 0.0, T * 4.0);
    
    // カメラ回転行列
    float cosAngle = cos(cos(T * 0.3) * 0.3);
    float sinAngle = sin(cos(T * 0.3) * 0.3);
    float2x2 rotMat = float2x2(
        cosAngle, -sinAngle,
        sinAngle, cosAngle
    );
    
    float2 rotatedUV = mul(u, rotMat);
    float3 D = normalize(float3(rotatedUV, 1.0));
    
    // レイマーチングループ
    for (i = 0.0; i < 100.0; i += 1.0)
    {
        // 各オーブとの距離を計算
        float o1 = Orb(p, T, 1.0, 0.1);
        float o2 = Orb(p, T, 2.0, 0.2);
        float o3 = Orb(p, T, 3.0, 0.3);
        float o4 = Orb(p, T, 4.0, 0.4);
        float o5 = Orb(p, T, 5.0, 0.5);
        
        e = max(min(o1, min(o2, min(o3, min(o4, o5)))), 0.001);
        
        // グリッドパターン計算用
        float3 q = p;
        q.y *= 0.6;
        b = abs(q.y) > 10.0;
        
        float3 q2 = abs(frac(q) - 0.5);
        
        float3 r2 = p; // rを保存
        
        // グリッドパターン
        m = sin(0.6 * T + r2.z * 0.1) > 0.0 
            ? min(q2.x, q2.y) 
            : max(q2.x, q2.y);
        
        // ステップサイズを計算
        s = min(e, (b ? 0.03 : 0.0) + 0.6 * max(abs(m), abs(0.4 + dot(sin(p / 6.0), cos(p.yzx / 16.0)))));
        
        // カラー蓄積
        c += float3(2, min(40.0 / d, 4.0), 3) / s + 20.0 * float3(0, 1, 30.0 / d) / max(e, 0.1);
        
        // レイを進める
        d += s;
        p += D * s;
    }
    
    // 最終カラー計算
    float3 finalColor = tanh3(c * c / 10000000.0 + 0.15 * dot(u, u));
    
    // 強度調整
    finalColor *= gScene.intensity;
    
    return float4(finalColor, 1.0);
}
