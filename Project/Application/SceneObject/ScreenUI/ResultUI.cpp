#include "ResultUI.h"
#include "Application/Utility/ApplicationGlobalValue.h"
#include "Application//Utility/MatsumotoUtility.h"
#include "Application/Utility/KeyBindConfig.h"
#include "Application/SceneObject/Score/ScoreCounter.h"

ResultUI::ResultUI(CoreEngine::BaseScene* baseScene, int* selectIndex) {
    baseScene_ = baseScene;
    selectIndex_ = selectIndex;
}

void ResultUI::Initialize() {
    // このシーンで使うスプライトオブジェクトの作成
    spriteObjects_.clear();
    spriteObjects_["BaseFrame"] = baseScene_->CreateObject<CoreEngine::SpriteObject>();
    spriteObjects_["BaseFrame"]->Initialize("Texture/UI_baseframe.png", "BaseUIFrame");
    spriteObjects_["BaseFrame"]->GetSpriteTransform().scale = { 1.05f,1.05f,1.05f };
    spriteObjects_["SubFrame"] = baseScene_->CreateObject<CoreEngine::SpriteObject>();
    spriteObjects_["SubFrame"]->Initialize("Texture/UI_subframe.png", "SubUIFrame");
    spriteObjects_["SubFrame"]->GetSpriteTransform().scale = { 1.05f,1.05f,1.05f };
    spriteObjects_["ScoreBoard"] = baseScene_->CreateObject<CoreEngine::SpriteObject>();
    spriteObjects_["ScoreBoard"]->Initialize("Texture/UI_score.png", "ScoreBoardUI");

    spriteObjects_["SelectTitle"] = baseScene_->CreateObject<CoreEngine::SpriteObject>();
    spriteObjects_["SelectTitle"]->Initialize("Texture/UI_title.png", "SelectTitleUI");
    spriteObjects_["SelectTitle"]->GetSpriteTransform().scale = { 0.0f,1.0f,1.0f };

    spriteObjects_["SelectRetry"] = baseScene_->CreateObject<CoreEngine::SpriteObject>();
    spriteObjects_["SelectRetry"]->Initialize("Texture/UI_return.png", "SelectRetryUI");
    spriteObjects_["SelectRetry"]->GetSpriteTransform().scale = { 0.0f,1.0f,1.0f };

    numberDisplay_ = std::make_unique<CoreEngine::NumberDisplayUtility>();
    numberDisplay_->Initialize(
        [this]() { return baseScene_->CreateObject<CoreEngine::SpriteObject>(); },
        "Texture/Number/", 8);

    spriteObjects_["S"] = baseScene_->CreateObject<CoreEngine::SpriteObject>();
    spriteObjects_["S"]->Initialize("Texture/UI_S.png", "RankUI");
    spriteObjects_["S"]->SetActive(false);
    spriteObjects_["S"]->GetSpriteTransform().scale = { 10.0f,10.0f,10.0f };
    spriteObjects_["A"] = baseScene_->CreateObject<CoreEngine::SpriteObject>();
    spriteObjects_["A"]->Initialize("Texture/UI_A.png", "RankUI");
    spriteObjects_["A"]->SetActive(false);
    spriteObjects_["A"]->GetSpriteTransform().scale = { 10.0f,10.0f,10.0f };
    spriteObjects_["B"] = baseScene_->CreateObject<CoreEngine::SpriteObject>();
    spriteObjects_["B"]->Initialize("Texture/UI_B.png", "RankUI");
    spriteObjects_["B"]->SetActive(false);
    spriteObjects_["B"]->GetSpriteTransform().scale = { 10.0f,10.0f,10.0f };

    // 色を設定（ゴールド）
    numberDisplay_->SetColor({ 1.0f, 0.9f, 0.3f, 1.0f });

    velocity_ = { 0.0f,0.0f };

    isAnimationStart_ = true;
    isAnimationEnd_ = false;
    isSoundPlayed_ = false;
    animationTimer_ = 0.0f;

    currentScore_ = 0;

    scoreDefaultPos_ = { -270.0f, 200.0f };

    CoreEngine::SoundManager* soundManager = baseScene_->GetEngineSystem()->GetComponent<CoreEngine::SoundManager>();
    if (!soundManager) {
        assert(false && "SoundManager not found");
    }
    soundResources_.clear();
    soundResources_["Score"] = soundManager->CreateSoundResource("ApplicationAssets/Sound/SE_Score.mp3");
    soundResources_["Stamp"] = soundManager->CreateSoundResource("ApplicationAssets/Sound/SE_Stamp.mp3");
}

void ResultUI::Update() {
    // アニメーションタイマー更新
    if (isAnimationStart_ && !isAnimationEnd_) {
        ScoreCounter& scoreCounter = ScoreCounter::GetInstance();
        if (currentScore_ < scoreCounter.GetScore()) {
            currentScore_ += (scoreCounter.GetScore() / 20) + 1;
            if (currentScore_ > scoreCounter.GetScore()) {
                currentScore_ = scoreCounter.GetScore();
            } else {

                if (!soundResources_["Score"]->IsPlaying()) {
                    soundResources_["Score"]->Play(false);
                }

            }
        }
        animationTimer_ += 0.016f;

        if (animationTimer_ > 1.0f) {
            if (!isSoundPlayed_) {
                soundResources_["Stamp"]->Play();
                isSoundPlayed_ = true;
            }
            
            // ランク表示
            if (scoreCounter.GetScore() >= ApplicationGlobalValue::SCORE_RANK_S) {
                spriteObjects_["S"]->SetActive(true);
                spriteObjects_["S"]->GetSpriteTransform().scale = MatsumotoUtility::SimpleEaseIn(
                    spriteObjects_["S"]->GetSpriteTransform().scale,
                    CoreEngine::Vector3(1.0f, 1.0f, 1.0f),
                    0.1f);
            } else if (scoreCounter.GetScore() >= ApplicationGlobalValue::SCORE_RANK_A) {
                spriteObjects_["A"]->SetActive(true);
                spriteObjects_["A"]->GetSpriteTransform().scale = MatsumotoUtility::SimpleEaseIn(
                    spriteObjects_["A"]->GetSpriteTransform().scale,
                    CoreEngine::Vector3(1.0f, 1.0f, 1.0f),
                    0.1f);
            } else {
                spriteObjects_["B"]->SetActive(true);
                spriteObjects_["B"]->GetSpriteTransform().scale = MatsumotoUtility::SimpleEaseIn(
                    spriteObjects_["B"]->GetSpriteTransform().scale,
                    CoreEngine::Vector3(1.0f, 1.0f, 1.0f),
                    0.1f);
            }
        }

        // アニメーション終了判定
        if (animationTimer_ >= 3.0f) {
            isAnimationEnd_ = true;
        }
    }

    if (isAnimationEnd_) {
        if (selectIndex_ != nullptr) {
            if (*selectIndex_ == 0) {
                spriteObjects_["SelectTitle"]->GetSpriteTransform().scale.x = MatsumotoUtility::SimpleEaseIn(
                    spriteObjects_["SelectTitle"]->GetSpriteTransform().scale.x,
                    0.8f,
                    0.3f);
                spriteObjects_["SelectRetry"]->GetSpriteTransform().scale.x = MatsumotoUtility::SimpleEaseIn(
                    spriteObjects_["SelectRetry"]->GetSpriteTransform().scale.x,
                    1.0f,
                    0.3f);
            } else {
                spriteObjects_["SelectTitle"]->GetSpriteTransform().scale.x = MatsumotoUtility::SimpleEaseIn(
                    spriteObjects_["SelectTitle"]->GetSpriteTransform().scale.x,
                    1.0f,
                    0.3f);
                spriteObjects_["SelectRetry"]->GetSpriteTransform().scale.x = MatsumotoUtility::SimpleEaseIn(
                    spriteObjects_["SelectRetry"]->GetSpriteTransform().scale.x,
                    0.8f,
                    0.3f);
            }
        }


    }

    // 入力取得
    float horizontalAxis = 0.0f;
    float verticalAxis = 0.0f;
    KeyBindConfig& keyConfig = KeyBindConfig::Instance();
    horizontalAxis = keyConfig.GetHorizontalAxis();
    verticalAxis = keyConfig.GetVerticalAxis();

    // 速度を減衰させる
    velocity_.x = MatsumotoUtility::SimpleEaseIn(
        velocity_.x,
        0.0f,
        0.1f);
    velocity_.y = MatsumotoUtility::SimpleEaseIn(
        velocity_.y,
        0.0f,
        0.1f);

    // UIをちょっと動かす
    float offsetAmount = 5.0f;
    for (auto& [name, spriteObject] : spriteObjects_) {
        spriteObject->GetSpriteTransform().translate.x =
            MatsumotoUtility::SimpleEaseIn(
                spriteObject->GetSpriteTransform().translate.x,
                horizontalAxis * offsetAmount + velocity_.x,
                0.1f);
        spriteObject->GetSpriteTransform().translate.y =
            MatsumotoUtility::SimpleEaseIn(
                spriteObject->GetSpriteTransform().translate.y,
                verticalAxis * offsetAmount + velocity_.y,
                0.1f);
        offsetAmount += 2.0f;
    }

    spriteObjects_["SelectTitle"]->GetSpriteTransform().translate.x = 100.0f;
    spriteObjects_["SelectRetry"]->GetSpriteTransform().translate.x = 100.0f;

    // スコア表示
    numberDisplay_->DisplayNumberWithDigits(
        currentScore_, scoreDefaultPos_ + CoreEngine::Vector2(horizontalAxis, verticalAxis), 6, { 1.5f, 1.5f }, 10.0f);

    // 数字表示の更新
    if (numberDisplay_) {
        numberDisplay_->Update();
    }
}
