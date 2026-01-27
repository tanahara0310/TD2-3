#pragma once
#include <memory>
#include "Scene/BaseScene.h"
#include "EngineSystem/EngineSystem.h"
#include "ObjectCommon/SpriteObject.h"
#include "Engine/Camera/CameraManager.h"
#include "Engine/Collider/CollisionConfig.h"
#include "Engine/Collider/CollisionManager.h"

#include "Application/Utility/Stopwatch.h"
#include "Application/Utility/Command/SceneCommandExecutor.h"
#include "Application/SceneObject/CameraController/CameraController.h"
#include "Application/SceneObject/Menu/MenuController.h"
#include "Application/SceneObject/Combo/EnemyKillComboCounter.h"
#include "Application/SceneObject/Combo/EnemyKillMotionManager.h"

class Player;
class Ball;
class WhiteSkyDome;
class Ground;
#include "Application/SceneObject/Menu/MenuView.h"
#include "Application/SceneObject/Ball/BallController.h"
#include "Application/SceneObject/Enemy/EnemyContainer.h"
#include "Application/SceneObject/Enemy/EnemyMapLoader.h"
#include "Application/SceneObject/Enemy/EnemyWaveManager.h"

#include "Application/SceneObject/GameRule/GameResultManager.h"
#include "Application/SceneObject/GameRule/TimeAndEnemyCountSpawnRule.h"
#include "Application/SceneObject/Presenters/GameClearSequence.h"

namespace CoreEngine
{
/// @brief ゲームシーンクラス
class GameScene : public BaseScene {
public:
	/// @brief 初期化
	void Initialize(EngineSystem* engine) override;

	/// @brief 描画処理
	void Draw() override;

	/// @brief 解放
	void Finalize() override;


    void NextWave();

protected:
	/// @brief 更新処理（BaseSceneのOnUpdate()をオーバーライド）
	void OnUpdate() override;

private:
    // スプライトの管理
    std::map<std::string, std::vector<SpriteObject*>> spriteObjects_;

    std::unique_ptr<CollisionConfig> collisionConfig_;
    std::unique_ptr<CollisionManager> collisionManager_;

    // シーンの操作
    SceneCommandExecutor sceneCommandExecutor_;
    // カメラの操作
    std::unique_ptr<CameraController> cameraController_;
    // ゲームの操作
    std::unique_ptr<MenuController> menuController_;

    // ゲームのオブジェクト
    Player* player_;
    Ball* ball_;
    WhiteSkyDome* skyDome_;
    Ground* ground_;
    std::unique_ptr<MenuView> menuView_;
    
    // ゲームオブジェクトの制御
    std::unique_ptr<Stopwatch> gameStopwatch_;
    std::unique_ptr<EnemyContainer> enemyManager_;
    std::unique_ptr<BallController> ballController_;
    std::unique_ptr<EnemyMapLoader> enemyMapLoader_;
    std::unique_ptr<EnemyKillComboCounter> enemyKillComboCounter_;
    std::unique_ptr<EnemyKillMotionManager> enemyKillMotionManager_;
    std::unique_ptr<EnemyWaveManager> enemyWaveManager_;

    std::unique_ptr<GameResultManager> gameResultManager_;
    std::unique_ptr<TimeAndEnemyCountSpawnRule> gameRule_;
    std::unique_ptr<GameClearSequence> gameClearSequence_;

    // BGM
    std::unique_ptr<CoreEngine::SoundManager::SoundResource> bgmSoundResource_;
};
}
