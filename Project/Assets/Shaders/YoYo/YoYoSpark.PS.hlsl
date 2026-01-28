// Spark Particle Shader

struct SparkMaterial
{
    float4 color;
    float time;
    float intensity;
    float2 padding;
};

cbuffer MaterialBuffer : register(b0)
{
    SparkMaterial gMaterial;
};

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

struct PixelShaderInput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float3 worldPosition : WORLD_POSITION;
};

PixelShaderOutput main(PixelShaderInput input)
{
    PixelShaderOutput output;
    
    // 中心からの距離
    float2 uv = input.texcoord * 2.0 - 1.0;
    float dist = length(uv);
    
    // 火花のような放射状パターン
    float angle = atan2(uv.y, uv.x);
    
    // 放射状の線（20本）
    float sparkCount = 20.0;
    float sparkAngle = frac(angle / (3.14159 * 2.0) * sparkCount);
    float sparkLine = smoothstep(0.45, 0.5, sparkAngle) - smoothstep(0.5, 0.55, sparkAngle);
    
    // 時間による明滅
    float flicker = sin(gMaterial.time * 10.0 + angle * 5.0) * 0.5 + 0.5;
    
    // 距離によるフェード
    float fade = 1.0 - smoothstep(0.0, 1.0, dist);
    fade = pow(fade, 2.0);
    
    // 最終的な明るさ
    float brightness = sparkLine * fade * flicker * gMaterial.intensity;
    
    // 色（オレンジ～黄色の火花）
    float3 sparkColor = lerp(float3(1.0, 0.5, 0.0), float3(1.0, 1.0, 0.3), flicker);
    
    output.color = float4(sparkColor * brightness * 2.0, brightness);
    
    return output;
}
