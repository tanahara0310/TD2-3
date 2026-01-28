#pragma once
#include <EngineSystem.h>
#include "Engine/Scene/BaseScene.h"
#include "ObjectCommon/SpriteObject.h"
#include <memory>
#include <map>

class Player;

class ScreenUI {
public:
    ~ScreenUI() = default;
    ScreenUI() = delete;
    explicit ScreenUI(CoreEngine::BaseScene* baseScene,Player* player);
    void Initialize();
    void Update();
private:
    CoreEngine::BaseScene* baseScene_;
    Player* player_;
    std::map<std::string, CoreEngine::SpriteObject*> spriteObjects_;
};