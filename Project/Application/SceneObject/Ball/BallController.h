#pragma once
#include "Engine/Math/Vector/Vector3.h"
#include <externals/nlohmann/single_include/nlohmann/json.hpp>
#include <functional>

class Player;
class Ball;

// ボールの挙動管理クラス
class BallController final {
public:
    BallController() = delete;
    explicit BallController(Ball* ball, Player* player);

    void Initialize();
    void Update();

    bool GetIsThrowing();

    void SetHitEffectFunction(
        const std::function<int(
            const CoreEngine::Vector3&,
            const CoreEngine::Vector3&,
            const CoreEngine::Vector3&)>& func) {
        hitEffectFunc_ = func;
    }
    void SetSlashEffectFunction(
        const std::function<int(
            const CoreEngine::Vector3&,
            const CoreEngine::Vector3&,
            const CoreEngine::Vector3&)>& func) {
        slashEffectFunc_ = func;
    }

private:
    Ball* ball_;
    Player* player_;

    CoreEngine::Vector3 anchorPos_;
    CoreEngine::Vector3 cartesianPos_;
    CoreEngine::Vector3 ballVelocity_;

    bool canSwitch_;
    bool isReturning_ = false;
    bool wasReturning_ = false;
    nlohmann::json config_;

    float hangTimeCounter_;
    float nowRadius_;

    float switchCooldown_;

    std::function<int(const CoreEngine::Vector3&, const CoreEngine::Vector3&, const CoreEngine::Vector3&)> hitEffectFunc_;
    std::function<int(const CoreEngine::Vector3&, const CoreEngine::Vector3&, const CoreEngine::Vector3&)> slashEffectFunc_;
};