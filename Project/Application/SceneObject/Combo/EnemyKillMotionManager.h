#pragma once
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
};