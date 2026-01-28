#include "ScreenUI.h"
#include "Application//Utility/MatsumotoUtility.h"
#include "Application/Utility/KeyBindConfig.h"
#include "Application/SceneObject/Player/Player.h"

ScreenUI::ScreenUI(CoreEngine::BaseScene* baseScene, Player* player) {
    baseScene_ = baseScene;
    player_ = player;
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
}

void ScreenUI::Update() {
    float horizontalAxis =
        player_->GetMoveAxis().x;
    float verticalAxis =
        player_->GetMoveAxis().y;

    float velocity = CoreEngine::Math::Vector::Length(player_->GetVelocity())*10.0f;

    // UIをちょっと動かす
    float offsetAmount = 5.0f;
    for (auto& [name, spriteObject] : spriteObjects_) {
        spriteObject->GetSpriteTransform().translate.x =
            MatsumotoUtility::SimpleEaseIn(
                spriteObject->GetSpriteTransform().translate.x,
                horizontalAxis * offsetAmount+ velocity,
                0.1f);
        spriteObject->GetSpriteTransform().translate.y =
            MatsumotoUtility::SimpleEaseIn(
                spriteObject->GetSpriteTransform().translate.y,
                verticalAxis * offsetAmount+ velocity,
                0.1f);
        offsetAmount += 5.0f;
    }
}
