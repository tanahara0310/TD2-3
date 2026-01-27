#pragma once
#include <functional>
#include <EngineSystem.h>

class EnemyKillComboCounter;
class Player;
class EnemyContainer;
class CameraController;
class BallController;

class EnemyKillMotionManager final {
public:
    EnemyKillMotionManager() = delete;
    explicit EnemyKillMotionManager(
        EnemyKillComboCounter* comboCounter,
        Player* player,
        EnemyContainer* enemyContainer,
        CameraController* cameraController,
        BallController* ballController);
    ~EnemyKillMotionManager() = default;

    void Update();

    // キルエフェクト実行関数の設定
    void SetKillEffectFunction(
        const std::function<int(
            const CoreEngine::Vector3&,
            const CoreEngine::Vector3&,
            const CoreEngine::Vector3&)>& func) {
        killEffectFunc_ = func;
    }

    // キル演出中かどうか
    bool isPlayingMotion_;

private:
    EnemyContainer* container_;
    EnemyKillComboCounter* comboCounter_;
    Player* player_;
    CameraController* cameraController_;
    BallController* ballController_;

    float eraseCooldown_;
    float currentEraseCooldown_;
    float eraseCooldownFactor_;

    std::function<int(const CoreEngine::Vector3&, const CoreEngine::Vector3&, const CoreEngine::Vector3&)> killEffectFunc_;
};