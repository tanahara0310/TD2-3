#pragma once
#include <EngineSystem.h>
#include "Engine/Scene/BaseScene.h"
#include "ObjectCommon/SpriteObject.h"
#include <memory>
#include <map>
#include "Engine/Utility/NumberDisplay/NumberDisplayUtility.h"

class ResultUI {
public:
    ResultUI() = delete;
    explicit ResultUI(CoreEngine::BaseScene* baseScene,int* selectIndex);
    ~ResultUI() = default;

    void Initialize();
    void Update();

    bool isAnimationStart_;
    bool isAnimationEnd_;

private:
    float animationTimer_;

    int currentScore_;
    int* selectIndex_;

    CoreEngine::BaseScene* baseScene_;
    std::map<std::string, CoreEngine::SpriteObject*> spriteObjects_;
    std::unique_ptr<CoreEngine::NumberDisplayUtility> numberDisplay_;
    CoreEngine::Vector2 velocity_;

    CoreEngine::Vector2 scoreDefaultPos_;
};