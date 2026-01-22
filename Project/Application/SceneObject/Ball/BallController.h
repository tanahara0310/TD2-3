#pragma once
#include "Engine/Math/Vector/Vector3.h"
#include <externals/nlohmann/single_include/nlohmann/json.hpp>

class Player;
class Ball;

// ボールの挙動管理クラス
class BallController final {
public:
    BallController() = delete;
    explicit BallController(Ball* ball, Player* player);

    void Update();

    bool GetIsThrowing();

private:
    Ball* ball_;
    Player* player_;

    CoreEngine::Vector3 anchorPos_;
    CoreEngine::Vector3 cartesianPos_;

    bool isReturning_ = false;
    nlohmann::json config_;

    float hangTimeCounter_;
    float nowRadius_;

    float switchCooldown_;
};