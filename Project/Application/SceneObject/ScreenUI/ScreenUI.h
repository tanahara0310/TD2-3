#pragma once
#include <EngineSystem.h>
#include "Engine/Scene/BaseScene.h"
#include "ObjectCommon/SpriteObject.h"
#include "Engine/Utility/NumberDisplay/NumberDisplayUtility.h"
#include <memory>
#include <map>

class Player;
class Stopwatch;
class BallController;
class MenuController;
enum class PlayerMode;

class ScreenUI {
public:
    ~ScreenUI() = default;
    ScreenUI() = delete;
    explicit ScreenUI(
        CoreEngine::BaseScene* baseScene,
        Player* player,Stopwatch* stopwatch,
        BallController*ball, MenuController* menuController);
    void Initialize();
    void Update();
private:
    CoreEngine::BaseScene* baseScene_;
    Player* player_;
    PlayerMode previousPlayerMode_;
    Stopwatch* stopwatch_;
    BallController* ball_;
    MenuController* menuController_;
    std::map<std::string, CoreEngine::SpriteObject*> spriteObjects_;
    std::unique_ptr < CoreEngine::NumberDisplayUtility> scoreDisplay_;
    CoreEngine::Vector2 scoreDefaultPos_;
    CoreEngine::Vector2 scorePos_;
    int currentScore_;
    bool isAnimationScore_;
    int scoreViewTimer_;

    bool isOldActiveBall_;
    float frameTimer_;
};