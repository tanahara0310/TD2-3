// UIFrame ポストエフェクト ピクセルシェーダー
Texture2D gTexture : register(t0);
SamplerState gSampler : register(s0);

// UIFrameパラメータ用の定数バッファ
cbuffer UIFrameParams : register(b0)
{
    float time;
    float padding1;
    float padding2;
    float padding3;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
};

struct PSOutput
{
    float4 color : SV_TARGET0;
};

// --- 定数定義 ---
static const float3 BORDER_COLOR = float3(1.0, 0.95, 0.0); // シアン→黄色に変更
static const float3 OUTSIDE_COLOR = float3(0.05, 0.05, 0.0); // ほぼ黒（わずかに黄色味）

static const float MARGIN_PCT = 0.02;  // 0.05 → 0.02 に縮小（エフェクトを大きく）
static const float CUT_SIZE = 10.0;
static const float BORDER_THICK_RATIO = 0.05;

static const float NOTCH_WIDTH_RATIO = 0.8;
static const float NOTCH_HEIGHT_RATIO = 0.025;
static const float NOTCH_ANGLE = 60.0;

static const float RIGHT_NOTCH_WIDTH_RATIO = 0.4;
static const float RIGHT_NOTCH_HEIGHT_RATIO = 0.02;
static const float RIGHT_NOTCH_ANGLE = 45.0;

static const float TL_TAB_HEIGHT_RATIO = 0.15;
static const float TL_TAB_WIDTH_RATIO = 0.6;
static const float TL_TAB_CHAMFER = 40.0;
static const float TL_GAP = 5.0;
static const float CORNER_RADIUS = 4.0;

static const float PATTERN_WIDTH = 10.0;
static const float PATTERN_GAP = 4.0;
static const float PATTERN_LINE_THICK = 1.0;
static const float PATTERN_HEIGHT_PCT = 0.75;

static const float BG_GRID_SIZE = 40.0;
static const float BG_GRID_THICKNESS = 1.0;

static const float PGBar_THICKNESS = 0.00005;
static const float PGBar_LENGTH = 0.3;
static const float PGBar_DIAMOND_SIZE = 0.005;

static const float3 PGBar_ACTIVE_COLOR = float3(1.0, 1.0, 0.0); // シアン→黄色に変更
static const float3 PGBar_INACTIVE_COLOR = float3(0.5, 0.5, 0.0); // 暗い黄色

static const float ELEC_SPEED = 4.0;
static const float ELEC_FREQUENCY = 4.0;
static const float ELEC_INTENSITY = 2.0;

static const float HEAD_DIAMOND_SIZE = 2.0;
static const float DIAMOND_ROT_SPEED = 5.0;

static const float DUR_FILL = 3.0;
static const float DUR_HOLD = 1.0;
static const float DUR_WIPE = 0.5;
static const float DUR_PAUSE = 0.5;

// --- ユーティリティ関数 ---
float hash(float n)
{
    return frac(sin(n) * 43758.5453123);
}

float noise(float x)
{
    float i = floor(x);
    float f = frac(x);
    float u = f * f * (3.0 - 2.0 * f);
    return lerp(hash(i), hash(i + 1.0), u);
}

float fbm(float x)
{
    float v = 0.0;
    float a = 0.5;
    float shift = 100.0;
    for (int i = 0; i < 3; ++i)
    {
        v += a * noise(x);
        x = x * 2.0 + shift;
        a *= 0.5;
    }
    return v;
}

float getNotchDist(float2 p, float width, float height, float angle)
{
    p.x = abs(p.x);
    float angleRad = radians(angle);
    float2 nWall = float2(sin(angleRad), cos(angleRad));
    float dWall = dot(p - float2(width * 0.5, 0.0), nWall);
    float dTop = p.y - height;
    return max(dWall, dTop);
}

float sdRhombus(float2 p, float size)
{
    return abs(p.x) + abs(p.y) - size;
}

float sdVerticalLine(float2 p, float height, float thickness)
{
    float2 d = abs(p) - float2(thickness, height);
    return min(max(d.x, d.y), 0.0) + length(max(d, 0.0));
}

float sdBox2D(float2 p, float2 b)
{
    float2 d = abs(p) - b;
    return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0);
}

float opRound(float d, float r)
{
    return d - r;
}

float sdTabShape(float2 p, float2 boxSize, float h, float w, float tlCut)
{
    float top = boxSize.y;
    float left = -boxSize.x;
    float bottom = boxSize.y - h;
    float dTop = p.y - top;
    float dLeft = -(p.x - left);
    float dBottom = -(p.y - bottom);
    float2 normalTL = float2(-0.7071, 0.7071);
    float2 originTL = float2(left + tlCut, top);
    float dTL = dot(p - originTL, normalTL);
    float2 normalRight = float2(0.7071, -0.7071);
    float2 originRight = float2(left + w, top);
    float dRight = dot(p - originRight, normalRight);
    float dShape = max(max(dTop, dLeft), max(dBottom, dRight));
    return max(dShape, dTL);
}

float2 getChamferedPoint(float angle, float2 boxSize, float cutSize, float radius)
{
    float2 rd = float2(sin(angle), cos(angle));
    float2 absRd = abs(rd);
    float2 targetBox = boxSize + radius;
    float2 tBoxVals = targetBox / max(absRd, 0.0001);
    float tBox = min(tBoxVals.x, tBoxVals.y);
    float distSharp = (boxSize.x + boxSize.y - cutSize) * 0.70710678;
    float distInflated = distSharp + radius;
    float denom = dot(absRd, float2(0.70710678, 0.70710678));
    float tChamfer = distInflated / max(denom, 0.0001);
    float t = min(tBox, tChamfer);
    return rd * t;
}

float sdMainFrame(float2 p, float2 boxSize, float2 iResolution)
{
    float2 dBoxVec = abs(p) - boxSize;
    float dBox = max(dBoxVec.x, dBoxVec.y);
    float dCorner = (abs(p.x) + abs(p.y) - (boxSize.x + boxSize.y - CUT_SIZE)) * 0.7071;
    float dFrame = max(dBox, dCorner);
    dFrame = opRound(dFrame, CORNER_RADIUS);
    
    float bNotchW = iResolution.y * NOTCH_WIDTH_RATIO;
    float bNotchH = iResolution.y * NOTCH_HEIGHT_RATIO;
    float rNotchW = iResolution.y * RIGHT_NOTCH_WIDTH_RATIO;
    float rNotchH = iResolution.y * RIGHT_NOTCH_HEIGHT_RATIO;
    
    float2 pBottom = float2(p.x, p.y + boxSize.y);
    float dNotchBottom = getNotchDist(pBottom, bNotchW, bNotchH, NOTCH_ANGLE);
    
    float2 pRight = float2(p.y, boxSize.x - p.x);
    float dNotchRight = getNotchDist(pRight, rNotchW, rNotchH, RIGHT_NOTCH_ANGLE);
    
    dFrame = max(dFrame, -min(dNotchBottom, dNotchRight));
    
    float tabH = iResolution.y * TL_TAB_HEIGHT_RATIO;
    float tabW = iResolution.y * TL_TAB_WIDTH_RATIO;
    float dTabHard = sdTabShape(p, boxSize, tabH, tabW, CUT_SIZE);
    float dSocket = opRound(dTabHard - TL_GAP, CORNER_RADIUS);
    dFrame = max(dFrame, -dSocket);
    
    return dFrame;
}

float2 snapToSurface(float2 pApprox, float2 boxSize, float2 iResolution)
{
    float2 p = pApprox;
    for (int i = 0; i < 3; i++)
    {
        float d = sdMainFrame(p, boxSize, iResolution);
        float2 e = float2(0.01, 0.0);
        float2 g = normalize(float2(
            sdMainFrame(p + e.xy, boxSize, iResolution) - sdMainFrame(p - e.xy, boxSize, iResolution),
            sdMainFrame(p + e.yx, boxSize, iResolution) - sdMainFrame(p - e.yx, boxSize, iResolution)
        ));
        p -= g * d;
    }
    return p;
}

float getRotatingDiamondDist(float2 p, float2 center, float size, float iTime)
{
    float2 offset = p - center;
    float rotAng = iTime * DIAMOND_ROT_SPEED;
    float c = cos(rotAng);
    float s = sin(rotAng);
    float2x2 rotMat = float2x2(c, -s, s, c);
    offset = mul(offset, rotMat);
    return abs(offset.x) + abs(offset.y) - size;
}

float3 getIlluminatingDiamondLight(float d)
{
    float dist = max(d, 0.0);
    float hotCore = 0.02 / (dist * dist + 0.005);
    hotCore *= smoothstep(1.0, 0.0, dist);
    float innerGlow = 0.6 / (dist + 0.2);
    innerGlow = pow(innerGlow, 2.5);
    float outerBloom = 1.0 / (dist * 0.3 + 1.0);
    outerBloom = pow(outerBloom, 2.0);
    float3 light = float3(0.0, 0.0, 0.0);
    light += float3(1.0, 1.0, 1.0) * hotCore * 1.5;
    light += BORDER_COLOR * innerGlow * 3.0;
    light += BORDER_COLOR * outerBloom * 0.8;
    return light;
}

PSOutput main(PSInput input)
{
    PSOutput output;
    
    // 元のテクスチャを取得
    float4 originalColor = gTexture.Sample(gSampler, input.texcoord);
    
    // 解像度を1280x720に固定（ポストエフェクト用）
    float2 iResolution = float2(1280.0, 720.0);
    float iTime = time;
    
    // ピクセル座標を計算
    float2 fragCoord = input.texcoord * iResolution;
    // DirectXとGLSLの座標系の違いを修正（Y軸を反転）
    fragCoord.y = iResolution.y - fragCoord.y;
    
    // レスポンシブ計算
    float borderThickness = iResolution.y * BORDER_THICK_RATIO;
    float tabH = iResolution.y * TL_TAB_HEIGHT_RATIO;
    float tabW = iResolution.y * TL_TAB_WIDTH_RATIO;
    
    // ジオメトリセットアップ
    float2 center = iResolution.xy * 0.5;
    float marginPx = iResolution.y * MARGIN_PCT;
    float2 boxSize = (iResolution.xy - float2(marginPx * 2.0, marginPx * 2.0)) * 0.5;
    float2 p = fragCoord - center;
    
    // 形状ロジック
    float dFrame = sdMainFrame(p, boxSize, iResolution);
    float dTabHard = sdTabShape(p, boxSize, tabH, tabW, CUT_SIZE);
    float dTab = opRound(dTabHard, CORNER_RADIUS);
    float dist = min(dFrame, dTab);
    
    // アニメーション＆電気効果
    float totalCycleTime = DUR_FILL + DUR_HOLD + DUR_WIPE + DUR_PAUSE;
    float timeInCycle = fmod(iTime, totalCycleTime);
    float cycleIndex = floor(iTime / totalCycleTime);
    float rnd = hash(cycleIndex);
    
    float cornerIndex = floor(rnd * 3.0);
    float startAngleBase = 0.785398 + (cornerIndex * 1.570796);
    
    float headPos = 0.0;
    float tailPos = 0.0;
    float wavePos = -1.0;
    float waveStrength = 0.0;
    
    if (timeInCycle < DUR_FILL)
    {
        headPos = timeInCycle / DUR_FILL;
        tailPos = 0.0;
    }
    else if (timeInCycle < (DUR_FILL + DUR_HOLD))
    {
        headPos = 1.0;
        tailPos = 0.0;
        float timeSinceFull = timeInCycle - DUR_FILL;
        if (timeSinceFull < 0.5)
        {
            wavePos = (timeSinceFull / 0.5) * 0.6;
            waveStrength = smoothstep(0.0, 1.0, 1.0 - (timeSinceFull / 0.5));
        }
    }
    else if (timeInCycle < (DUR_FILL + DUR_HOLD + DUR_WIPE))
    {
        float wipeTime = timeInCycle - (DUR_FILL + DUR_HOLD);
        headPos = 1.0;
        tailPos = wipeTime / DUR_WIPE;
    }
    else
    {
        headPos = 1.0;
        tailPos = 1.0;
    }
    
    float angle = atan2(p.x, p.y);
    float shiftedAngle = angle - startAngleBase;
    float normalizedPos = frac(shiftedAngle / 6.283185);
    float mask = step(tailPos, normalizedPos) * step(normalizedPos, headPos);
    
    float pulseFactor = 0.0;
    if (wavePos >= 0.0)
    {
        float distFromStart = min(normalizedPos, 1.0 - normalizedPos);
        pulseFactor = exp(-abs(distFromStart - wavePos) * 10.0) * waveStrength;
    }
    
    float electricSignal = fbm(angle * ELEC_FREQUENCY + iTime * ELEC_SPEED);
    electricSignal = pow(abs(electricSignal), 3.0) * mask;
    
    float absDist = abs(dist);
    float currentThickness = borderThickness * (1.0 + pulseFactor * 3.0);
    float glow = 1.0 / (1.0 + absDist * (10.0 / currentThickness));
    glow = pow(glow, 2.0);
    
    float3 baseColor = lerp(BORDER_COLOR, float3(1.0, 1.0, 1.0), pulseFactor * 0.8);
    float totalIntensity = glow * (1.0 + electricSignal * ELEC_INTENSITY);
    
    if (mask < 0.5)
    {
        totalIntensity *= 0.3;
        baseColor = BORDER_COLOR;
    }
    
    // ダイヤモンドロジック
    float2 posStartApprox = getChamferedPoint(startAngleBase, boxSize, CUT_SIZE, CORNER_RADIUS);
    float headAngle = startAngleBase + (headPos * 6.283185);
    float2 posHeadApprox = getChamferedPoint(headAngle, boxSize, CUT_SIZE, CORNER_RADIUS);
    
    float2 posStart = snapToSurface(posStartApprox, boxSize, iResolution);
    float2 posHead = snapToSurface(posHeadApprox, boxSize, iResolution);
    
    float3 totalDiamondLight = float3(0.0, 0.0, 0.0);
    float maxDiamondIntensity = 0.0;
    
    // 静止ダイヤモンド
    if (tailPos < 0.1)
    {
        float dStartDia = getRotatingDiamondDist(p, posStart, HEAD_DIAMOND_SIZE, iTime);
        float3 light = getIlluminatingDiamondLight(dStartDia);
        float fade = 1.0 - smoothstep(0.0, 0.1, tailPos);
        totalDiamondLight += light * fade;
        maxDiamondIntensity = max(maxDiamondIntensity, light.g * fade);
    }
    
    // 移動ダイヤモンド
    if (headPos > 0.01 && headPos < 0.99)
    {
        float dHeadDia = getRotatingDiamondDist(p, posHead, HEAD_DIAMOND_SIZE, iTime);
        float3 light = getIlluminatingDiamondLight(dHeadDia);
        totalDiamondLight += light;
        maxDiamondIntensity = max(maxDiamondIntensity, light.g);
    }
    
    // ステータスバー
    float2 pBar = p;
    pBar.x -= boxSize.x;
    float barLenPx = iResolution.y * PGBar_LENGTH * 0.5;
    float barThickPx = iResolution.y * PGBar_THICKNESS;
    float diaSizePx = iResolution.y * PGBar_DIAMOND_SIZE;
    
    float dBarShape = min(sdVerticalLine(pBar, barLenPx, barThickPx),
                          min(sdRhombus(pBar - float2(0.0, barLenPx), diaSizePx),
                              sdRhombus(pBar - float2(0.0, -barLenPx), diaSizePx)));
    
    float3 barColor = float3(0.0, 0.0, 0.0);
    float barAlpha = 0.0;
    
    if (dBarShape < 1.0)
    {
        float LINE_PROGRESS = 0.5 + 0.5 * sin(iTime * 2.0);
        float normalizedY = 0.5 - (pBar.y / (2.0 * barLenPx));
        bool isActive = normalizedY < LINE_PROGRESS;
        
        if (sdRhombus(pBar - float2(0.0, barLenPx), diaSizePx) < 1.0) 
            isActive = true;
        if (sdRhombus(pBar - float2(0.0, -barLenPx), diaSizePx) < 1.0) 
            isActive = (LINE_PROGRESS > 0.99);
        
        if (isActive)
        {
            barColor = PGBar_ACTIVE_COLOR * (1.0 + 0.5 / (1.0 + abs(dBarShape) * 0.5));
        }
        else
        {
            barColor = PGBar_INACTIVE_COLOR;
        }
        
        barAlpha = 1.0 - smoothstep(0.0, 1.5, dBarShape);
    }
    
    // 合成＆ヘッダーパターン
    float4 finalResult;
    totalIntensity = max(totalIntensity, maxDiamondIntensity);
    
    if (dist > 0.0)
    {
        // 背景（外側・4隅）- ダークグレーで塗りつぶす
        finalResult = float4(OUTSIDE_COLOR, 1.0);
    }
    else
    {
        // 内部（Land）- UIフレームを描画
        // 基本背景色を黒に設定
        float3 col = float3(0.0, 0.0, 0.0);
        
        // ヘッダーパターン（島のみ）
        if (dTab < 0.0 && p.y > (boxSize.y - CUT_SIZE))
        {
            float skewX = p.x - p.y;
            float totalCellSize = PATTERN_WIDTH + PATTERN_GAP;
            float localX = (frac(skewX / totalCellSize) - 0.5) * totalCellSize;
            float headerMidY = boxSize.y - (CUT_SIZE * 0.5);
            float localY = p.y - headerMidY;
            float contentHeight = CUT_SIZE * PATTERN_HEIGHT_PCT;
            float halfH = contentHeight * 0.5;
            float halfW = PATTERN_WIDTH * 0.5;
            float innerHalfH = halfH - (PATTERN_LINE_THICK * 0.5);
            float dPara = sdBox2D(float2(localX, localY), float2(halfW, innerHalfH));
            float lineDist = abs(dPara) - (PATTERN_LINE_THICK * 0.5);
            float lineAlpha = 1.0 - smoothstep(0.0, 1.0, lineDist);
            col = lerp(float3(0.0, 0.0, 0.0), BORDER_COLOR, lineAlpha);
            totalIntensity = max(totalIntensity, lineAlpha);
        }
        
        // Landグリッド（メインフレームのみ）
        if (dTab > 0.0)
        {
            float2 gridUV = abs(frac(p / BG_GRID_SIZE) - 0.5);
            float distToLine = min(gridUV.x, gridUV.y) * BG_GRID_SIZE;
            float gridAlpha = 1.0 - smoothstep(0.0, BG_GRID_THICKNESS, distToLine);
            float maskBorder = smoothstep(2.0, 10.0, absDist);
            gridAlpha *= maskBorder;
            // グリッドを追加（黒背景の上に黄色のグリッド）
            col += BORDER_COLOR * 0.35 * gridAlpha;
        }
        
        // ダイヤモンドの光を追加
        col += totalDiamondLight;
        
        // 枠の部分にグロー効果を追加（電気効果）
        float3 borderGlow = baseColor * totalIntensity;
        col += borderGlow;
        
        // 電気効果の追加輝度
        col += BORDER_COLOR * electricSignal * glow * 0.5;
        
        // 元のテクスチャとブレンド
        finalResult = float4(col, 1.0);
    }
    
    finalResult.rgb = lerp(finalResult.rgb, barColor, barAlpha);
    finalResult.a = max(finalResult.a, barAlpha);
    
    output.color = finalResult;
    return output;
}
