#include "TitleUIManager.h"
#include "Application/Utility/MatsumotoUtility.h"
#include "Engine/Math/Easing/EasingUtil.h"
#include <cmath>

using namespace CoreEngine;

void TitleUIManager::Initialize(std::function<SpriteObject* ()> createSpriteFunc)
{
    auto* titleFrameSprite = createSpriteFunc();
    titleFrameSprite->Initialize("Assets/Texture/Title/TitleFrame.png", "TitleFrame");
    titleFrameSprite->GetSpriteTransform().translate = { 0.0f, 0.0f, 10.0f };  // Z座標を設定（最背面）
    titleFrameSprite->SetAnchor({ 0.5f, 0.5f });
    titleFrameSprite->GetSpriteTransform().scale = { 1.0f, 1.0f, 1.0f };
    titleFrameSprite_ = std::make_unique<SpriteObject*>(titleFrameSprite);

    // タイトル画像の作成（画面外上からスタート）
    auto* titleSprite = createSpriteFunc();
    titleSprite->Initialize("Assets/Texture/Title/title.png", "TitleLogo");
    titleSprite->GetSpriteTransform().translate = { 0.0f, 800.0f, 0.0f };
    titleSprite->SetAnchor({ 0.5f, 0.5f });
    titleSprite->GetSpriteTransform().scale = { 0.1f, 0.1f, 1.0f };
    titleSprite_ = std::make_unique<SpriteObject*>(titleSprite);

    // スペーススタート画像の作成
    auto* spaceStartSprite = createSpriteFunc();
    spaceStartSprite->Initialize("Assets/Texture/Title/start.png", "start");
    spaceStartSprite->GetSpriteTransform().translate = { 0.0f, -200.0f, 0.0f };
    spaceStartSprite->SetAnchor({ 0.5f, 0.5f });
    spaceStartSprite->GetSpriteTransform().scale = { 0.6f, 0.6f, 0.6f };
    spaceStartSprite_ = std::make_unique<SpriteObject*>(spaceStartSprite);


    // 左側の指スプライトの作成（タイトルロゴの左側）
    auto* leftHandSprite = createSpriteFunc();
    leftHandSprite->Initialize("Assets/Texture/Title/taitleHande.png", "LeftHand");
    leftHandSprite->GetSpriteTransform().translate = { 0.0f, 0.0f, 0.0f };
    leftHandSprite->SetAnchor({ 0.5f, 0.5f });
    leftHandSprite->GetSpriteTransform().scale = { 0.4f, 0.4f, 1.0f };
    leftHandSprite_ = std::make_unique<SpriteObject*>(leftHandSprite);

    // 右側の指スプライトの作成（タイトルロゴの右側、反転）
    auto* rightHandSprite = createSpriteFunc();
    rightHandSprite->Initialize("Assets/Texture/Title/taitleHande.png", "RightHand");
    rightHandSprite->GetSpriteTransform().translate = { 0.0f, 0.0f, 0.0f };
    rightHandSprite->SetAnchor({ 0.5f, 0.5f });
    rightHandSprite->GetSpriteTransform().scale = { -0.4f, 0.4f, 1.0f };  // X軸を反転
    rightHandSprite_ = std::make_unique<SpriteObject*>(rightHandSprite);


    // タイマーを初期化
    Reset();
}

void TitleUIManager::Update(float deltaTime)
{
    if (!titleSprite_ || !spaceStartSprite_) return;

    // タイマー更新
    animationTimer_.Update(deltaTime);
    pulseTimer_.Update(deltaTime);
    glitchTimer_.Update(deltaTime);
    blinkTimer_.Update(deltaTime);
    handTimer_.Update(deltaTime);
    spinTimer_.Update(deltaTime);

    // 退出アニメーション中
    if (isExitAnimationPlaying_) {
        exitTimer_.Update(deltaTime);
        UpdateExitAnimation();
        return;  // 退出アニメーション中は他のアニメーションを停止
    }

    // イントロアニメーション
    if (!isIntroAnimationComplete_) {
        introTimer_.Update(deltaTime);
        UpdateIntroAnimation();
    } else {
        UpdateMainAnimation();

        // タイトルロゴの回転更新
        UpdateTitleRotation(deltaTime);
    }

    // スペーススタートのアニメーション（常に実行）
    UpdateSpaceStartAnimation();

    // 指のアニメーション（常に実行）
    UpdateHandAnimation();
}

void TitleUIManager::Reset()
{
    // 全タイマーをリセット
    animationTimer_.Reset();
    pulseTimer_.Reset();
    glitchTimer_.Reset();
    blinkTimer_.Reset();
    introTimer_.Reset();
    handTimer_.Reset();
    spinTimer_.Reset();
    exitTimer_.Reset();

    // 連続動作用タイマーを開始
    animationTimer_.Start(9999.0f, false);  // 長時間動作
    pulseTimer_.Start(9999.0f, false);
    glitchTimer_.Start(9999.0f, false);
    blinkTimer_.Start(9999.0f, false);
    handTimer_.Start(9999.0f, false);
    spinTimer_.Start(9999.0f, false);
    introTimer_.Start(1.0f, false);  // イントロは1秒

    isIntroAnimationComplete_ = false;
    titleRotationVelocity_ = 0.0f;
    spinCycleTime_ = 0.0f;
    rotationDecayTimer_ = 0.0f;
    totalRotationTime_ = 0.0f;
    initialRotationSpeed_ = 0.0f;
    isRotationDecaying_ = false;
    isExitAnimationPlaying_ = false;
    hasTriggeredShake_ = false;
}

void TitleUIManager::UpdateIntroAnimation()
{
    if (!titleSprite_) return;

    float t = introTimer_.GetProgress();

    // イージング（EaseOutBack）で飛び込んでくる
    float easeT = 1.0f - std::pow(1.0f - t, 3.0f);
    float overshoot = 1.70158f;
    float overshootT = 1.0f + overshoot * std::pow(t - 1.0f, 3.0f) + overshoot * std::pow(t - 1.0f, 2.0f);

    // タイトルロゴが上から飛び込む + 回転しながら拡大（スケール0.8倍）
    (*titleSprite_)->GetSpriteTransform().translate.y = MatsumotoUtility::Lerp(800.0f, 200.0f, overshootT);
    (*titleSprite_)->GetSpriteTransform().scale.x = MatsumotoUtility::Lerp(0.1f, 0.8f, easeT);
    (*titleSprite_)->GetSpriteTransform().scale.y = MatsumotoUtility::Lerp(0.1f, 0.8f, easeT);
    (*titleSprite_)->GetSpriteTransform().rotate.z = MatsumotoUtility::Lerp(MatsumotoUtility::DegreesToRadians(720.0f), 0.0f, easeT);

    if (introTimer_.IsFinished()) {
        isIntroAnimationComplete_ = true;
    }
}

void TitleUIManager::UpdateMainAnimation()
{
    if (!titleSprite_) return;

    // 1. パルス/鼓動エフェクト（タイトルロゴ）- ベーススケール0.8倍
    float pulseTime = pulseTimer_.GetElapsedTime();
    float pulseScale = 0.8f + std::sin(pulseTime * 2.0f) * 0.04f;
    (*titleSprite_)->GetSpriteTransform().scale.x = pulseScale;
    (*titleSprite_)->GetSpriteTransform().scale.y = pulseScale;

    // 2. 微細な回転（エネルギーを放出している感じ）
    float animTime = animationTimer_.GetElapsedTime();
    float rotationOffset = std::sin(animTime * 1.5f) * MatsumotoUtility::DegreesToRadians(3.0f);
    (*titleSprite_)->GetSpriteTransform().rotate.z = rotationOffset;

    // 3. グリッチエフェクト（3秒ごとにリセット）
    float glitchTime = glitchTimer_.GetElapsedTime();
    float glitchCycle = fmod(glitchTime, 3.0f);

    if (glitchCycle < 0.1f) {
        float glitchOffset = (std::sin(glitchCycle * 100.0f) * 10.0f);
        (*titleSprite_)->GetSpriteTransform().translate.x = glitchOffset;
    } else {
        (*titleSprite_)->GetSpriteTransform().translate.x = 0.0f;
    }
}

void TitleUIManager::UpdateSpaceStartAnimation()
{
    if (!spaceStartSprite_) return;

    // スペーススタートの点滅（アーケード風）
    float blinkTime = blinkTimer_.GetElapsedTime();
    float blinkAlpha = (std::sin(blinkTime * 3.0f) + 1.0f) * 0.5f;
    (*spaceStartSprite_)->SetColor({ 1.0f, 1.0f, 1.0f, blinkAlpha });

    // スペーススタートの上下振動
    float animTime = animationTimer_.GetElapsedTime();
    float floatOffset = std::sin(animTime * 2.0f) * 10.0f;
    (*spaceStartSprite_)->GetSpriteTransform().translate.y = -200.0f + floatOffset;
}

void TitleUIManager::UpdateHandAnimation()
{
    if (!leftHandSprite_ || !rightHandSprite_) return;

    float handTime = handTimer_.GetElapsedTime();

    // 回転演出のサイクル（6秒周期）
    const float cycleDuration = 6.0f;
    float previousCycleTime = spinCycleTime_;
    spinCycleTime_ = fmod(handTime, cycleDuration);

    // サイクルがリセットされたら、回転状態もリセット（次の回転を可能にする）
    if (previousCycleTime > spinCycleTime_) {
        isRotationDecaying_ = false;
        rotationDecayTimer_ = 0.0f;
        hasTriggeredShake_ = false;  // シェイクフラグもリセット
    }

    // === 回転演出のフェーズ分け ===
    // Phase 0 (0.0-1.5s): 待機状態（軽い揺れ）
    // Phase 1 (1.5-2.5s): 予備動作（引く）
    // Phase 2 (2.5-3.0s): タイトルロゴを回転させる（押し出す）
    // Phase 3 (3.0-6.0s): 元の位置に戻る

    float baseLeftX = -420.0f;   // 左側の指の基準位置（タイトルロゴから離す）
    float baseRightX = 420.0f;   // 右側の指の基準位置（タイトルロゴから離す）
    float baseY = 200.0f;

    float leftX = baseLeftX;
    float rightX = baseRightX;
    float leftRotation = 0.0f;
    float rightRotation = 0.0f;
    float scale = 0.4f;

    if (spinCycleTime_ < 1.5f) {
        // Phase 0: 待機状態（軽い揺れ）
        float idleTime = spinCycleTime_;
        leftX = baseLeftX + std::sin(idleTime * 3.0f) * 10.0f;
        rightX = baseRightX + std::sin(idleTime * 3.0f) * 10.0f;
        leftRotation = std::sin(idleTime * 2.0f) * MatsumotoUtility::DegreesToRadians(5.0f);
        rightRotation = -leftRotation;

    } else if (spinCycleTime_ < 2.5f) {
        // Phase 1: 予備動作（引く）
        float t = (spinCycleTime_ - 1.5f) / 1.0f;  // 0→1
        float easeT = std::sin(t * 3.14159f * 0.5f);  // EaseOutSine

        // 左右に引く
        leftX = MatsumotoUtility::Lerp(baseLeftX, baseLeftX - 80.0f, easeT);
        rightX = MatsumotoUtility::Lerp(baseRightX, baseRightX + 80.0f, easeT);

        // 回転（力を溜める）
        leftRotation = MatsumotoUtility::Lerp(0.0f, MatsumotoUtility::DegreesToRadians(-20.0f), easeT);
        rightRotation = MatsumotoUtility::Lerp(0.0f, MatsumotoUtility::DegreesToRadians(20.0f), easeT);

        // スケールも少し大きく
        scale = MatsumotoUtility::Lerp(0.4f, 0.5f, easeT);

    } else if (spinCycleTime_ < 3.0f) {
        // Phase 2: タイトルロゴを回転させる（押し出す）
        float t = (spinCycleTime_ - 2.5f) / 0.5f;  // 0→1
        float easeT = 1.0f - std::pow(1.0f - t, 3.0f);  // EaseOutCubic

        // 素早くタイトルロゴに向かって押し出す
        leftX = MatsumotoUtility::Lerp(baseLeftX - 80.0f, baseLeftX + 50.0f, easeT);
        rightX = MatsumotoUtility::Lerp(baseRightX + 80.0f, baseRightX - 50.0f, easeT);

        // 回転（押し出す動作）
        leftRotation = MatsumotoUtility::Lerp(MatsumotoUtility::DegreesToRadians(-20.0f),
            MatsumotoUtility::DegreesToRadians(15.0f), easeT);
        rightRotation = MatsumotoUtility::Lerp(MatsumotoUtility::DegreesToRadians(20.0f),
            MatsumotoUtility::DegreesToRadians(-15.0f), easeT);

        scale = MatsumotoUtility::Lerp(0.5f, 0.45f, easeT);

        // この瞬間にタイトルロゴに回転速度を与える（回転が終わっている場合のみ）
        if (t > 0.3f && !isRotationDecaying_ && titleRotationVelocity_ < 0.1f) {
            titleRotationVelocity_ = MatsumotoUtility::DegreesToRadians(3600.0f);  // 超高速回転開始（10回転）
            
            // このサイクルで一度だけエフェクトを発動
            if (!hasTriggeredShake_) {
                // カメラシェイクを発動（微弱な揺れ）
                if (cameraShakeCallback_) {
                    cameraShakeCallback_(0.3f, 2.5f);  // duration: 0.3秒, intensity: 2.5（微弱）
                }

                // ショックウェーブを発動（画面中央、小さめの強度）
                if (shockwaveCallback_) {
                    shockwaveCallback_(0.5f, 0.5f, 0.3f);  // centerX: 0.5, centerY: 0.5, intensity: 0.3（小さめ）
                }

                // パーティクルを発生
                if (particleSpawnCallback_) {
                    particleSpawnCallback_();
                }
                
                hasTriggeredShake_ = true;  // このサイクルでは1回のみ
            }
        }

    } else {
        // Phase 3: 元の位置に戻る
        float t = (spinCycleTime_ - 3.0f) / 3.0f;  // 0→1
        float easeT = std::sin(t * 3.14159f * 0.5f);  // EaseOutSine

        leftX = MatsumotoUtility::Lerp(baseLeftX + 50.0f, baseLeftX, easeT);
        rightX = MatsumotoUtility::Lerp(baseRightX - 50.0f, baseRightX, easeT);

        leftRotation = MatsumotoUtility::Lerp(MatsumotoUtility::DegreesToRadians(15.0f), 0.0f, easeT);
        rightRotation = MatsumotoUtility::Lerp(MatsumotoUtility::DegreesToRadians(-15.0f), 0.0f, easeT);

        scale = MatsumotoUtility::Lerp(0.45f, 0.4f, easeT);
    }

    // 上下の微細な動き（常に適用）
    float verticalOffset = std::sin(handTime * 2.0f) * 8.0f;

    // === 左側の指を更新 ===
    (*leftHandSprite_)->GetSpriteTransform().translate.x = leftX;
    (*leftHandSprite_)->GetSpriteTransform().translate.y = baseY + verticalOffset;
    (*leftHandSprite_)->GetSpriteTransform().rotate.z = leftRotation;
    (*leftHandSprite_)->GetSpriteTransform().scale.x = scale;
    (*leftHandSprite_)->GetSpriteTransform().scale.y = scale;

    // === 右側の指を更新 ===
    (*rightHandSprite_)->GetSpriteTransform().translate.x = rightX;
    (*rightHandSprite_)->GetSpriteTransform().translate.y = baseY + verticalOffset;
    (*rightHandSprite_)->GetSpriteTransform().rotate.z = rightRotation;
    (*rightHandSprite_)->GetSpriteTransform().scale.x = -scale;  // 反転を維持
    (*rightHandSprite_)->GetSpriteTransform().scale.y = scale;

    // 点滅（両方の指に適用）
    float blinkTime = blinkTimer_.GetElapsedTime();
    float blinkAlpha = 0.7f + (std::sin(blinkTime * 3.0f) * 0.3f);
    (*leftHandSprite_)->SetColor({ 1.0f, 1.0f, 1.0f, blinkAlpha });
    (*rightHandSprite_)->SetColor({ 1.0f, 1.0f, 1.0f, blinkAlpha });
}

void TitleUIManager::UpdateTitleRotation(float deltaTime)
{
    if (!titleSprite_) return;

    const float PI = 3.14159265f;
    const float TWO_PI = 2.0f * PI;

    // 回転開始トリガー
    if (!isRotationDecaying_ && titleRotationVelocity_ > 0.1f) {
        isRotationDecaying_ = true;
        rotationDecayTimer_ = 0.0f;
        totalRotationTime_ = 3.0f;  // 3秒かけて減速
        initialRotationSpeed_ = titleRotationVelocity_;
        titleRotationVelocity_ = 0.0f;  // velocityベースから時間ベースに切り替え
    }

    if (isRotationDecaying_) {
        rotationDecayTimer_ += deltaTime;
        float t = rotationDecayTimer_ / totalRotationTime_;

        if (t < 1.0f) {
            // Phase 1: EaseOutQuintで滑らかに減速
            float easedT = EasingUtil::Apply(t, EasingUtil::Type::EaseOutQuint);

            // 初速度から0まで減速する速度カーブ
            float currentSpeed = initialRotationSpeed_ * (1.0f - easedT);

            // 現在の回転速度で回転
            float currentRotationX = (*titleSprite_)->GetSpriteTransform().rotate.x;
            currentRotationX += currentSpeed * deltaTime;
            (*titleSprite_)->GetSpriteTransform().rotate.x = currentRotationX;

        } else {
            // Phase 2: 回転が止まったら、0度に戻す（最短経路でスムーズに）
            float currentRotationX = (*titleSprite_)->GetSpriteTransform().rotate.x;

            // 角度を-π〜πの範囲に正規化
            float normalizedRotation = currentRotationX;
            while (normalizedRotation > PI) {
                normalizedRotation -= TWO_PI;
            }
            while (normalizedRotation < -PI) {
                normalizedRotation += TWO_PI;
            }

            // 差が小さければそのまま0に
            if (std::abs(normalizedRotation) < MatsumotoUtility::DegreesToRadians(0.5f)) {
                (*titleSprite_)->GetSpriteTransform().rotate.x = 0.0f;
                isRotationDecaying_ = false;  // 回転完全停止
            } else {
                // EaseOutQuadで0度に収束
                float returnT = (rotationDecayTimer_ - totalRotationTime_) / 1.0f;  // 1秒かけて戻る
                if (returnT > 1.0f) returnT = 1.0f;

                float easedReturnT = EasingUtil::Apply(returnT, EasingUtil::Type::EaseOutQuad);

                // 正規化された角度から0度に向かって補間（最短経路）
                float targetRotation = normalizedRotation * (1.0f - easedReturnT);
                (*titleSprite_)->GetSpriteTransform().rotate.x = targetRotation;
            }
        }
    }
}

void TitleUIManager::StartExitAnimation()
{
    if (isExitAnimationPlaying_) return;  // 既に実行中なら何もしない

    isExitAnimationPlaying_ = true;
    exitTimer_.Reset();
    exitTimer_.Start(1.0f, false);  // 1.0秒のアニメーション（高速化）
}

void TitleUIManager::UpdateExitAnimation()
{
    if (!spaceStartSprite_) return;

    float t = exitTimer_.GetProgress();
    const float baseY = -200.0f;  // 元のY座標

    // === Phase 0 (0.0-0.15s): 押し込み（予備動作） ===
    if (t < 0.15f) {
        float phaseT = t / 0.15f;  // 0→1

        // EaseInQuadで押し込む
        float squashT = EasingUtil::Apply(phaseT, EasingUtil::Type::EaseInQuad);

        // 少し下に移動
        float currentY = MatsumotoUtility::Lerp(baseY, baseY - 30.0f, squashT);
        (*spaceStartSprite_)->GetSpriteTransform().translate.y = currentY;

        // スケールを少し小さく（押し込まれた感じ）
        float scaleX = MatsumotoUtility::Lerp(0.6f, 0.55f, squashT);
        float scaleY = MatsumotoUtility::Lerp(0.6f, 0.65f, squashT);  // Y方向は少し伸ばす
        (*spaceStartSprite_)->GetSpriteTransform().scale.x = scaleX;
        (*spaceStartSprite_)->GetSpriteTransform().scale.y = scaleY;

        // Y軸回転はまだ開始しない
        (*spaceStartSprite_)->GetSpriteTransform().rotate.y = 0.0f;

    } else if (t < 0.45f) {
        // === Phase 1 (0.15-0.45s): ジャンプアップ + Y軸高速回転 ===
        float phaseT = (t - 0.15f) / 0.3f;  // 0→1

        // EaseOutQuadで上にジャンプ
        float jumpT = EasingUtil::Apply(phaseT, EasingUtil::Type::EaseOutQuad);
        float jumpHeight = 400.0f;  // ジャンプの高さ
        float currentY = MatsumotoUtility::Lerp(baseY - 30.0f, baseY + jumpHeight, jumpT);
        (*spaceStartSprite_)->GetSpriteTransform().translate.y = currentY;

        // Y軸回転のみ（高速回転）
        float rotationSpeedY = MatsumotoUtility::DegreesToRadians(1800.0f);  // 5回転/秒（高速化）
        float currentRotationY = rotationSpeedY * phaseT * 0.3f;  // 0.3秒分の回転
        (*spaceStartSprite_)->GetSpriteTransform().rotate.y = currentRotationY;

        // スケールを元に戻しながら少し大きく
        float scaleT = EasingUtil::Apply(phaseT, EasingUtil::Type::EaseOutCubic);
        float scale = MatsumotoUtility::Lerp(0.55f, 0.75f, scaleT);
        (*spaceStartSprite_)->GetSpriteTransform().scale.x = scale;
        (*spaceStartSprite_)->GetSpriteTransform().scale.y = scale;

    } else if (t < 0.7f) {
        // === Phase 2 (0.45-0.7s): 元の位置に戻る ===
        float phaseT = (t - 0.45f) / 0.25f;  // 0→1

        // EaseInOutQuadで元の位置に戻る
        float returnT = EasingUtil::Apply(phaseT, EasingUtil::Type::EaseInOutQuad);
        float currentY = MatsumotoUtility::Lerp(baseY + 400.0f, baseY, returnT);
        (*spaceStartSprite_)->GetSpriteTransform().translate.y = currentY;

        // Y軸回転を継続（Phase 1の最終回転量を維持）
        float rotationSpeedY = MatsumotoUtility::DegreesToRadians(1800.0f);
        float phase1RotationY = rotationSpeedY * 0.3f;  // Phase 1の最終回転量
        (*spaceStartSprite_)->GetSpriteTransform().rotate.y = phase1RotationY;

        // スケールを元に戻す
        float scale = MatsumotoUtility::Lerp(0.75f, 0.6f, returnT);
        (*spaceStartSprite_)->GetSpriteTransform().scale.x = scale;
        (*spaceStartSprite_)->GetSpriteTransform().scale.y = scale;

    } else {
        // === Phase 3 (0.7-1.0s): 回転を0度に収束 ===
        float phaseT = (t - 0.7f) / 0.3f;  // 0→1

        // 位置は元の位置で固定
        (*spaceStartSprite_)->GetSpriteTransform().translate.y = baseY;

        // Y軸回転をEaseOutQuadで0度に収束（正規化せずに直接0度へ）
        float rotationSpeedY = MatsumotoUtility::DegreesToRadians(1800.0f);
        float phase1RotationY = rotationSpeedY * 0.3f;  // Phase 1の最終回転量

        // EaseOutQuadで直接0度に向かって補間
        float easeT = EasingUtil::Apply(phaseT, EasingUtil::Type::EaseOutQuad);
        float currentRotationY = phase1RotationY * (1.0f - easeT);  // 直接0度に向かって減少
        (*spaceStartSprite_)->GetSpriteTransform().rotate.y = currentRotationY;

        // スケールは元のサイズで固定
        (*spaceStartSprite_)->GetSpriteTransform().scale.x = 0.6f;
        (*spaceStartSprite_)->GetSpriteTransform().scale.y = 0.6f;
    }
}






