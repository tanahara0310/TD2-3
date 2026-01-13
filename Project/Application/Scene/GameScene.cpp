#include "GameScene.h"
#include "Scene/SceneManager.h"

#include "Application/SceneObjects/Player/Player.h"
#include "Application/SceneObjects/CameraController/AllCameraWork.h"

GameScene::GameScene() {
    player_ = CreateObject<Player>();
    mapManager_ = CreateObject<MapManager>();
    defaultCameraPos_ = { 0.0f, 15.0f, -15.0f };
}

void GameScene::Initialize(EngineSystem* engine) {
    BaseScene::Initialize(engine);
    
    // マップの初期化（10x10）
    mapManager_->Initialize(10, 10);

    cameraController_.Initialize();
    cameraController_.SetCameraWork<FollowCamera>(*(cameraManager_.get()),player_->GetTransform(), defaultCameraPos_, 0.1f);
}

void GameScene::Update() {
    cameraController_.Update();
    BaseScene::Update();
}

void GameScene::Draw() {
    BaseScene::Draw();
}

void GameScene::Finalize() {
    BaseScene::Finalize();
}
