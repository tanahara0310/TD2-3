#include "GameUIManager.h"
#include "Engine/EngineSystem/EngineSystem.h"
#include "Engine/WinApp/WinApp.h"
#include "Engine/ObjectCommon/SpriteObject.h"
#include "Engine/Utility/FrameRate/FrameRateController.h"
#include "Application/AL3/GameObject/PlayerObject.h"
#include "Application/AL3/GameObject/BossObject.h"
#include "Application/AL3/GameObject/ReticleObject.h"

void GameUIManager::Initialize(
    EngineSystem* engine,
    PlayerObject* player,
    BossObject* boss,
    ReticleObject* reticle
) {
    engine_ = engine;
    player_ = player;
    boss_ = boss;
    reticle_ = reticle;
}

void GameUIManager::InitializeBossName(std::function<SpriteObject* (const std::string&, const std::string&)> createSpriteCallback) {
    if (!createSpriteCallback) return;

    // ボス名画像を画面上部中央に配置
    bossNameSprite_ = createSpriteCallback("Assets/Texture/bossName.png", "BossName");
    bossNameSprite_->SetActive(true);
    bossNameSprite_->GetSpriteTransform().translate = { 0.0f, 321.0f, 0.0f };
    bossNameSprite_->GetSpriteTransform().scale = { 0.6f, 0.6f, 1.0f };
}

void GameUIManager::InitializeBossHPBar(std::function<SpriteObject*(const std::string&, const std::string&)> createSpriteCallback) {
    if (!createSpriteCallback) return;

    // 枠の太さ
    constexpr float FRAME_PADDING = 3.0f;  // 4 → 3に縮小

    // HPバー背景（黒）- 枠として少し大きく
    bossHPBarBackground_ = createSpriteCallback("Assets/Texture/white1x1.png", "BossHPBarBackground");
    bossHPBarBackground_->SetActive(true);
    bossHPBarBackground_->GetSpriteTransform().translate = { 0.0f, HP_BAR_Y, 0.1f };  // Z=0.1で一番奥
    bossHPBarBackground_->GetSpriteTransform().scale = { HP_BAR_WIDTH + FRAME_PADDING * 2, HP_BAR_HEIGHT + FRAME_PADDING * 2, 1.0f };
    bossHPBarBackground_->SetColor({ 0.0f, 0.0f, 0.0f, 1.0f });  // 黒色に変更

    // HPバー遅延（赤・ダメージ表示用）- 背景と前景の間に配置
    bossHPBarDelay_ = createSpriteCallback("Assets/Texture/white1x1.png", "BossHPBarDelay");
    bossHPBarDelay_->SetActive(true);
    bossHPBarDelay_->GetSpriteTransform().translate = { 0.0f, HP_BAR_Y, 0.05f };  // Z=0.05で中間
    bossHPBarDelay_->GetSpriteTransform().scale = { HP_BAR_WIDTH, HP_BAR_HEIGHT, 1.0f };
    bossHPBarDelay_->SetColor({ 0.8f, 0.1f, 0.1f, 1.0f });  // 暗めの赤

    // HPバー前景（緑）
    bossHPBarForeground_ = createSpriteCallback("Assets/Texture/white1x1.png", "BossHPBarForeground");
    bossHPBarForeground_->SetActive(true);
    bossHPBarForeground_->GetSpriteTransform().translate = { 0.0f, HP_BAR_Y, 0.0f };  // Z=0.0で一番手前
    bossHPBarForeground_->GetSpriteTransform().scale = { HP_BAR_WIDTH, HP_BAR_HEIGHT, 1.0f };
    bossHPBarForeground_->SetColor({ 0.2f, 0.8f, 0.2f, 1.0f });  // 鮮やかな緑
    
    // 初期値として遅延HPを最大に設定
    delayedHPRatio_ = 1.0f;
}

void GameUIManager::Update() {
    if (!boss_ || !bossHPBarForeground_ || !bossHPBarDelay_) {
        return;
    }

    // deltaTimeを取得
    auto frameRateController = engine_->GetComponent<FrameRateController>();
    if (!frameRateController) {
        return;
    }
    float deltaTime = frameRateController->GetDeltaTime();

    // 現在のHP割合を取得
    float currentHPRatio = boss_->GetHPRatio();

    // 遅延HPを更新（常に現在HPと比較して大きい方を保持し、徐々に減少）
    if (delayedHPRatio_ > currentHPRatio) {
        // 遅延HPが現在HPより大きい場合は徐々に減少
        delayedHPRatio_ -= HP_DELAY_SPEED * deltaTime;
        
        // 現在のHPを下回らないようにクランプ
        if (delayedHPRatio_ < currentHPRatio) {
            delayedHPRatio_ = currentHPRatio;
        }
    }
    // 新しくダメージを受けた場合（currentHPRatioが遅延HPより小さくなった場合）
    // 遅延HPはそのまま維持され、徐々に減少していく
    // これにより連続ダメージでも赤いバーが引き継がれる

    // 前景バー（緑）の更新
    bossHPBarForeground_->GetSpriteTransform().scale.x = HP_BAR_WIDTH * currentHPRatio;
    float foregroundOffset = HP_BAR_WIDTH * (1.0f - currentHPRatio) * -0.5f;
    bossHPBarForeground_->GetSpriteTransform().translate.x = foregroundOffset;

    // 遅延バー（赤）の更新
    bossHPBarDelay_->GetSpriteTransform().scale.x = HP_BAR_WIDTH * delayedHPRatio_;
    float delayOffset = HP_BAR_WIDTH * (1.0f - delayedHPRatio_) * -0.5f;
    bossHPBarDelay_->GetSpriteTransform().translate.x = delayOffset;
}
