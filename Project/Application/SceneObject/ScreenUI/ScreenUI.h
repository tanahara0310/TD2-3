#pragma once
#include <EngineSystem.h>
#include "Engine/Scene/BaseScene.h"
#include "ObjectCommon/SpriteObject.h"
#include <memory>
#include <map>

class ScreenUI {
public:
    ~ScreenUI() = default;
    ScreenUI() = delete;
    explicit ScreenUI(CoreEngine::BaseScene* baseScene);
    void Initialize();
    void Update();
private:
    CoreEngine::BaseScene* baseScene_;
    std::map<std::string, CoreEngine::SpriteObject*> spriteObjects_;
};