#include "ScreenUI.h"
#include "Application//Utility/MatsumotoUtility.h"
#include "Application/Utility/KeyBindConfig.h"

ScreenUI::ScreenUI(CoreEngine::BaseScene* baseScene) {
    baseScene_ = baseScene;
}

void ScreenUI::Initialize() {
    // このシーンで使うスプライトオブジェクトの作成
    spriteObjects_.clear();
    spriteObjects_["BaseFrame"] = baseScene_->CreateObject<CoreEngine::SpriteObject>();
    spriteObjects_["BaseFrame"]->Initialize("Texture/UI_baseframe.png", "BaseUIFrame");
    spriteObjects_["SubFrame"] = baseScene_->CreateObject<CoreEngine::SpriteObject>();
    spriteObjects_["SubFrame"]->Initialize("Texture/UI_subframe.png", "SubUIFrame");
    spriteObjects_["SubFrame"]->GetSpriteTransform().scale = { 1.05f,1.05f,1.05f };
    spriteObjects_["HandFrame"] = baseScene_->CreateObject<CoreEngine::SpriteObject>();
    spriteObjects_["HandFrame"]->Initialize("Texture/UI_handframe.png", "HandUIFrame");
}

void ScreenUI::Update() {
    float horizontalAxis =
        KeyBindConfig::Instance().GetHorizontalAxis();
    float verticalAxis =
        KeyBindConfig::Instance().GetVerticalAxis();

    // UIをちょっと動かす
    spriteObjects_["SubFrame"]->GetSpriteTransform().translate.x =
        MatsumotoUtility::SimpleEaseIn(
            spriteObjects_["SubFrame"]->GetSpriteTransform().translate.x,
            horizontalAxis * 10.0f,
            0.1f);
    spriteObjects_["SubFrame"]->GetSpriteTransform().translate.y =
        MatsumotoUtility::SimpleEaseIn(
            spriteObjects_["SubFrame"]->GetSpriteTransform().translate.y,
            verticalAxis * -10.0f,
            0.1f);
}
