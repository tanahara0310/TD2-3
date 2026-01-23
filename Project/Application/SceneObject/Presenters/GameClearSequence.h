#pragma once
#include "Scene/BaseScene.h"
#include "EngineSystem/EngineSystem.h"
#include "ObjectCommon/SpriteObject.h"
#include "Application/Utility/Command/SceneCommandExecutor.h"
#include "Engine/Scene/SceneManager.h"

class GameClearSequence {
public:
    GameClearSequence() = delete;
    explicit GameClearSequence(
        CoreEngine::BaseScene* baseScene,
        CoreEngine::SceneManager* sceneManager,
        SceneCommandExecutor* sceneCommandExecutor);
    ~GameClearSequence() = default;

    void Initialize();
    void Update();
    void Finalize();

private:
    CoreEngine::BaseScene* baseScene_;
    CoreEngine::SceneManager* sceneManager_;
    SceneCommandExecutor* sceneCommandExecutor_;

    bool isSelectingTitle_;
    float animTimer_;

    CoreEngine::SpriteObject* gameClearSprite_;
    CoreEngine::SpriteObject* titleSprite_;
    CoreEngine::SpriteObject* retrySprite_;
};