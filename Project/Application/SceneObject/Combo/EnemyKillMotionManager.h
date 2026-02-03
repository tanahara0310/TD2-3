#pragma once
#include <functional>
#include <EngineSystem.h>

namespace CoreEngine {
    class ParticleSystem;
}

class EnemyKillComboCounter;
class Player;
class EnemyContainer;
class CameraController;
class BallController;
class Stopwatch;

class EnemyKillMotionManager final {
public:
    EnemyKillMotionManager() = delete;
    explicit EnemyKillMotionManager(
        EnemyKillComboCounter* comboCounter,
        Player* player,
        EnemyContainer* enemyContainer,
        CameraController* cameraController,
        BallController* ballController,
        Stopwatch* gameTimer);
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

    // パーティクルプールの設定
    void SetEnemyDeathParticlePool(std::vector<CoreEngine::ParticleSystem*>* pool) {
        enemyDeathParticlePool_ = pool;
    }

private:
    EnemyContainer* container_;
    EnemyKillComboCounter* comboCounter_;
    Player* player_;
    CameraController* cameraController_;
    BallController* ballController_;
    Stopwatch* gameTimer_;

    float eraseCooldown_;
    float currentEraseCooldown_;
    float eraseCooldownFactor_;

    std::function<int(const CoreEngine::Vector3&, const CoreEngine::Vector3&, const CoreEngine::Vector3&)> killEffectFunc_;
    
    std::vector<CoreEngine::ParticleSystem*>* enemyDeathParticlePool_ = nullptr;

    // パーティクルプールから使用可能なパーティクルを取得
    CoreEngine::ParticleSystem* GetAvailableParticle();
};