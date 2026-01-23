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
    titleSprite_ = baseScene_->CreateObject<CoreEngine::SpriteObject>();
    retrySprite_ = baseScene_->CreateObject<CoreEngine::SpriteObject>();
    gameClearSprite_->Initialize("Texture/uvChecker.png");
    titleSprite_->Initialize("Texture/uvChecker.png");
    retrySprite_->Initialize("Texture/uvChecker.png");

    isSelectingTitle_ = true;
    animTimer_ = 2.0f;
}

void GameClearSequence::Initialize() {
    gameClearSprite_->GetSpriteTransform().translate.y = -720.0f;
    gameClearSprite_->GetSpriteTransform().scale.y = 0.5f;

    titleSprite_->GetSpriteTransform().translate.y = 720.0f;
    titleSprite_->GetSpriteTransform().scale.y = 0.2f;
    titleSprite_->GetSpriteTransform().scale.x = 0.7f;
    retrySprite_->GetSpriteTransform().translate.y = 720.0f;
    retrySprite_->GetSpriteTransform().scale.y = 0.2f;
    retrySprite_->GetSpriteTransform().scale.x = 0.7f;

    animTimer_ = 3.0f;
}

void GameClearSequence::Update() {
    gameClearSprite_->GetSpriteTransform().translate.y =
        MatsumotoUtility::SimpleEaseIn(gameClearSprite_->GetSpriteTransform().translate.y, 300.0f, 0.1f);
    //titleSprite_->GetSpriteTransform().translate.y =
    //    MatsumotoUtility::SimpleEaseIn(titleSprite_->GetSpriteTransform().translate.y, 0.0f, 0.1f);
    //retrySprite_->GetSpriteTransform().translate.y =
    //    MatsumotoUtility::SimpleEaseIn(retrySprite_->GetSpriteTransform().translate.y, -200.0f, 0.1f);

    if (animTimer_ > 0.0f) {
        animTimer_ -= 1.0f/60.0f;
        return;
    }

    sceneCommandExecutor_->
        AddCommand(std::make_unique<SceneChangeCommand>("ResultScene", sceneManager_));

    //KeyBindConfig& keyBindConfig = KeyBindConfig::Instance();

    //if (keyBindConfig.IsTrigger("MoveForward") || keyBindConfig.IsTrigger("MoveBack")) {
    //    isSelectingTitle_ = !isSelectingTitle_;
    //}

    //if (isSelectingTitle_) {
    //    titleSprite_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
    //    retrySprite_->SetColor({ 0.5f,0.5f,0.5f,1.0f });

    //    titleSprite_->GetSpriteTransform().scale.x =
    //        MatsumotoUtility::SimpleEaseIn(titleSprite_->GetSpriteTransform().scale.x, 0.8f, 0.1f);
    //    retrySprite_->GetSpriteTransform().scale.x =
    //        MatsumotoUtility::SimpleEaseIn(retrySprite_->GetSpriteTransform().scale.x, 0.7f, 0.1f);

    //} else {
    //    titleSprite_->SetColor({ 0.5f,0.5f,0.5f,1.0f });
    //    retrySprite_->SetColor({ 1.0f,1.0f,1.0f,1.0f });

    //    titleSprite_->GetSpriteTransform().scale.x =
    //        MatsumotoUtility::SimpleEaseIn(titleSprite_->GetSpriteTransform().scale.x, 0.7f, 0.1f);
    //    retrySprite_->GetSpriteTransform().scale.x =
    //        MatsumotoUtility::SimpleEaseIn(retrySprite_->GetSpriteTransform().scale.x, 0.8f, 0.1f);
    //}

    //if (keyBindConfig.IsTrigger("Start")) {
    //    if (isSelectingTitle_) {
    //        // シーン変更コマンドを追加（タイトルシーンへ）
    //        sceneCommandExecutor_->AddCommand(
    //            std::make_unique<SceneChangeCommand>("TitleScene", sceneManager_));
    //    } else {
    //        // リトライコマンドを追加
    //        sceneCommandExecutor_->AddCommand(
    //            std::make_unique<SceneChangeCommand>("GameScene", sceneManager_));
    //    }

    //}
}

void GameClearSequence::Finalize() {

}
