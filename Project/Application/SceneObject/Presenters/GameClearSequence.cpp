#include "GameClearSequence.h"
#include "Application/Utility/Command/SceneAllCommand.h"
#include "Application/Utility/MatsumotoUtility.h"
#include "Application/Utility/KeyBindConfig.h"

GameClearSequence::GameClearSequence(
    CoreEngine::BaseScene* baseScene,
    CoreEngine::SceneManager* sceneManager,
    SceneCommandExecutor* sceneCommandExecutor) {
    baseScene_ = baseScene;
    sceneManager_ = sceneManager;
    sceneCommandExecutor_ = sceneCommandExecutor;

    gameClearSprite_ = baseScene_->CreateObject<CoreEngine::SpriteObject>();
    gameClearSprite_->Initialize("Texture/UI_timeup.png");

    isSelectingTitle_ = true;
    animTimer_ = 2.0f;
}

void GameClearSequence::Initialize() {
    gameClearSprite_->GetSpriteTransform().translate.y = -720.0f;

    animTimer_ = 3.0f;
}

void GameClearSequence::Update() {
    gameClearSprite_->GetSpriteTransform().translate.y =
        MatsumotoUtility::SimpleEaseIn(gameClearSprite_->GetSpriteTransform().translate.y, 0.0f, 0.1f);

    if (animTimer_ > 0.0f) {
        animTimer_ -= 1.0f/60.0f;
        return;
    }

    sceneCommandExecutor_->
        AddCommand(std::make_unique<SceneChangeCommand>("ResultScene", sceneManager_));
}

void GameClearSequence::Finalize() {

}
