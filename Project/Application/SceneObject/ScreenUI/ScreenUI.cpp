#include "ScreenUI.h"
#include "Application//Utility/MatsumotoUtility.h"
#include "Application/Utility/KeyBindConfig.h"
#include "Application/SceneObject/Player/Player.h"
#include "Application/Utility/Stopwatch.h"

#include "Application/Utility/ApplicationGlobalValue.h"

ScreenUI::ScreenUI(CoreEngine::BaseScene* baseScene, Player* player, Stopwatch* stopwatch) {
    baseScene_ = baseScene;
    player_ = player;
    stopwatch_ = stopwatch;
}

void ScreenUI::Initialize() {
    // このシーンで使うスプライトオブジェクトの作成
    spriteObjects_.clear();
    spriteObjects_["BaseFrame"] = baseScene_->CreateObject<CoreEngine::SpriteObject>();
    spriteObjects_["BaseFrame"]->Initialize("Texture/UI_baseframe.png", "BaseUIFrame");
    spriteObjects_["BaseFrame"]->GetSpriteTransform().scale = { 1.05f,1.05f,1.05f };
    spriteObjects_["SubFrame"] = baseScene_->CreateObject<CoreEngine::SpriteObject>();
    spriteObjects_["SubFrame"]->Initialize("Texture/UI_subframe.png", "SubUIFrame");
    spriteObjects_["SubFrame"]->GetSpriteTransform().scale = { 1.05f,1.05f,1.05f };
    spriteObjects_["HandFrame"] = baseScene_->CreateObject<CoreEngine::SpriteObject>();
    spriteObjects_["HandFrame"]->Initialize("Texture/UI_handframe.png", "HandUIFrame");
    spriteObjects_["HandFrame"]->GetSpriteTransform().scale = { 1.05f,1.05f,1.05f };
    spriteObjects_["TempScore"] = baseScene_->CreateObject<CoreEngine::SpriteObject>();
    spriteObjects_["TempScore"]->Initialize("Texture/UI_score.png", "TempScoreUI");
    spriteObjects_["TempScore"]->GetSpriteTransform().scale = { 1.0f,1.0f,1.0f };
    spriteObjects_["ControlGuide"] = baseScene_->CreateObject<CoreEngine::SpriteObject>();
    spriteObjects_["ControlGuide"]->Initialize("Texture/UI_controll.png", "ControlGuideUI");
    spriteObjects_["ControlGuide"]->GetSpriteTransform().scale = { 1.0f,1.0f,1.0f };

    // 時間表示用ヨーヨー
    spriteObjects_["YoYo"] = baseScene_->CreateObject<CoreEngine::SpriteObject>();
    spriteObjects_["YoYo"]->Initialize("Texture/yoyo.png", "YoYoUI");

    // 手
    spriteObjects_["Hand"] = baseScene_->CreateObject<CoreEngine::SpriteObject>();
    spriteObjects_["Hand"]->Initialize("Texture/hand.png", "HandUI");

}

void ScreenUI::Update() {
    float horizontalAxis =
        player_->GetMoveAxis().x;
    float verticalAxis =
        player_->GetMoveAxis().y;

    float velocity = CoreEngine::Math::Vector::Length(player_->GetVelocity()) * 10.0f;

    // デフォルト位置に戻す
    float timeRatio = static_cast<float>(stopwatch_->ElapsedMilliseconds() / ApplicationGlobalValue::GAME_CLEAR_TIME_MS);
    spriteObjects_["YoYo"]->GetSpriteTransform().rotate.z += 5.0f;
    spriteObjects_["YoYo"]->GetSpriteTransform().translate = { -450.0f,MatsumotoUtility::Lerp(-250.0f,100.0f,timeRatio),0.0f };
    spriteObjects_["Hand"]->GetSpriteTransform().translate = { -550.0f,280.0f,0.0f };

    // UIをちょっと動かす
    float offsetAmount = 5.0f;
    for (auto& [name, spriteObject] : spriteObjects_) {
        spriteObject->GetSpriteTransform().translate.x =
            MatsumotoUtility::SimpleEaseIn(
                spriteObject->GetSpriteTransform().translate.x,
                horizontalAxis * offsetAmount + velocity,
                0.1f);
        spriteObject->GetSpriteTransform().translate.y =
            MatsumotoUtility::SimpleEaseIn(
                spriteObject->GetSpriteTransform().translate.y,
                verticalAxis * offsetAmount + velocity,
                0.1f);
        offsetAmount += 2.0f;
    }
}
