#include "TitleScene.h"
#include "Scene/SceneManager.h"

#include <memory>
#include "Application/Utility/Command/SceneAllCommand.h"
#include "Application/Utility/KeyBindConfig.h"

void TitleScene::Initialize(EngineSystem* engine) {
    BaseScene::Initialize(engine);
    frameStartCommandExecutor_.Initialize();
}

void TitleScene::Update() {
    frameStartCommandExecutor_.ExecuteCommand();

    if (KeyBindConfig::Instance().IsTrigger("Start")) {
        frameStartCommandExecutor_.AddCommand(std::make_unique<SceneChangeCommand>("GameScene", sceneManager_));
    }

    BaseScene::Update();
}

void TitleScene::Draw() {
    BaseScene::Draw();
}

void TitleScene::Finalize() {
    BaseScene::Finalize();
}
