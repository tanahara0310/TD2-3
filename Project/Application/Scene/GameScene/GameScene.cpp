#include "GameScene.h"
#include "EngineSystem.h"
#include "Scene/SceneManager.h"
#include "Engine/Graphics/Render/RenderManager.h"
#include "Engine/Graphics/TextureManager.h"

#include "Application/Utility/MatsumotoUtility.h"

// アプリケーションのユーティリティ
#include "Application/Utility/KeyBindConfig.h"
#include "Application/SceneObject/CameraController/AllCameraWork.h"
// アプリケーションのシーンオブジェクト
#include "Application/SceneObject/Player/Player.h"
#include "Application/SceneObject/Ball/Ball.h"
#include "Application/SceneObject/Enemy/AllEnemy.h"
#include "Application/SceneObject/SkyDome/WhiteSkyDome.h"

#include "Application/Utility/Command/SceneAllCommand.h"

namespace {
    const double GAME_CLEAR_TIME_MS = 60000.0;
}

namespace CoreEngine
{
void GameScene::Initialize(EngineSystem* engine)
{
	BaseScene::Initialize(engine);

	// コンポーネントを直接取得
	auto dxCommon = engine_->GetComponent<DirectXCommon>();
	auto renderManager = engine_->GetComponent<RenderManager>();

	if (!dxCommon || !renderManager) {
		return;
	}

    // このシーンで使うスプライトオブジェクトの作成
    spriteObjects_.clear();

	// ゲームシーンの初期化処理
    sceneCommandExecutor_.Initialize();
    cameraController_ = std::make_unique<CameraController>(cameraManager_.get());
    cameraController_->Initialize();
    gameStopwatch_ = std::make_unique<Stopwatch>();
    gameStopwatch_->Start();

    // ゲームオブジェクトの生成
    player_ = CreateObject<Player>();
    player_->SetAutoUpdate(false);
    ball_ = CreateObject<Ball>();
    ball_->SetAutoUpdate(false);

    skyDome_ = CreateObject<WhiteSkyDome>();
    skyDome_->SetColor(MatsumotoUtility::ColorEggplant);

    // プレイヤーの初期化
    player_->Initialize();
    ball_->Initialize();

    enemyManager_ = std::make_unique<EnemyContainer>(this);

    // ボールコントローラーの生成
    ballController_ = std::make_unique<BallController>(ball_, player_);
    ballController_->Initialize();
    // メニューコントローラーの生成
    menuController_ = std::make_unique<MenuController>(sceneCommandExecutor_);
    menuController_->Initialize();
    menuView_ = std::make_unique<MenuView>(this, menuController_.get());
    menuView_->Initialize();
    // 敵配置データのロード
    enemyMapLoader_ = std::make_unique<EnemyMapLoader>(enemyManager_.get());
    enemyMapLoader_->LoadEnemyMap("testA.json");
    enemyMapLoader_->LoadEnemyMap("testB.json");
    enemyMapLoader_->LoadEnemyMap("testC.json");

    // 敵キルコンボカウンターの生成
    enemyKillComboCounter_ = std::make_unique<EnemyKillComboCounter>(enemyManager_.get());
    enemyKillComboCounter_->Initialize();
    // 敵キルモーションマネージャーの生成
    enemyKillMotionManager_ = std::make_unique<EnemyKillMotionManager>(
        enemyKillComboCounter_.get(),
        player_,
        enemyManager_.get(),
        cameraController_.get(),
        ballController_.get());
    enemyKillMotionManager_->isPlayingMotion_ = false;

    // 敵ウェーブマネージャーの生成
    enemyWaveManager_ = std::make_unique<EnemyWaveManager>(enemyMapLoader_.get());

    collisionConfig_ = std::make_unique<CollisionConfig>();
    collisionManager_ = std::make_unique<CollisionManager>(collisionConfig_.get());

    collisionConfig_->SetCollisionEnabled(CollisionLayer::Player, CollisionLayer::Enemy, true);
    collisionConfig_->SetCollisionEnabled(CollisionLayer::Enemy, CollisionLayer::Item, true);

    collisionManager_->Clear();
    collisionManager_->RegisterCollider(player_->GetCollider());
    collisionManager_->RegisterCollider(ball_->GetCollider());
    auto& enemyMap = enemyManager_->GetEnemyMap();
    for (auto& [typeName, enemyList] : enemyMap) {
        for (auto& enemy : enemyList) {
            collisionManager_->RegisterCollider(enemy->GetCollider());
        }
    }

    // ゲーム結果マネージャーの生成
    gameResultManager_ = std::make_unique<GameResultManager>();
    gameResultManager_->Initialize();
    // 時間経過でクリア
    std::function<bool()> timeUpCondition = [this]() {
        return gameStopwatch_->ElapsedMilliseconds() >= GAME_CLEAR_TIME_MS;
        };
    gameResultManager_->AddGameClearCondition(timeUpCondition);
    
    // ゲームルールの設定
    gameRule_ = std::make_unique<TimeAndEnemyCountSpawnRule>(
        std::bind(&GameScene::NextWave,this) ,
        std::bind(&EnemyContainer::GetAliveEnemyCount,enemyManager_.get()));

    // ゲームクリアシーケンスの生成
    gameClearSequence_ = std::make_unique<GameClearSequence>(this, sceneManager_, &sceneCommandExecutor_);
    gameClearSequence_->Initialize();

    // BGM
    auto soundManager = engine_->GetComponent<SoundManager>();
    if (soundManager) {
        bgmSoundResource_ = soundManager->CreateSoundResource("ApplicationAssets/Sound/BGM_InGame.mp3");
        if (bgmSoundResource_) {
            bgmSoundResource_->Play(true);
            bgmSoundResource_->SetVolume(0.2f);
        }
    }
}

void GameScene::OnUpdate()
{
#ifdef _DEBUG
    ImGui::Begin("Game Controller");
    // 時間の表示
    ImGui::Text("Elapsed Time: %.2f ms", gameStopwatch_->ElapsedMilliseconds());
    // バーで表示
    float timeRatio = static_cast<float>(gameStopwatch_->ElapsedMilliseconds() / GAME_CLEAR_TIME_MS);
    ImGui::ProgressBar(timeRatio, ImVec2(0.0f, 0.0f), "Time to Clear");
    ImGui::End();
#endif

    // 入力処理更新
    KeyBindConfig::Instance().Update();

    if (gameResultManager_->CheckGameClear()) {
        gameClearSequence_->Update();
        sceneCommandExecutor_.ExecuteCommand();
        return;
        //menuController_->
    }

    // メニューコントローラーの更新
    menuController_->Update();
    menuView_->Update();

    // メニューが閉じている場合のみゲームシーンを更新
    if (!menuController_->IsMenuOpen()) {
        if(!gameStopwatch_->IsRunning()){
            gameStopwatch_->Resume();
        }

        cameraController_->Update();
        if (!enemyKillMotionManager_->isPlayingMotion_) {
            player_->Update();
            ball_->Update();
            ballController_->Update();
            gameRule_->Update();
            
        }
        enemyManager_->Update();

        enemyKillComboCounter_->Update();
        enemyKillMotionManager_->Update();

        collisionManager_->CheckAllCollisions();
    } else {
        if (gameStopwatch_->IsRunning()) {
            gameStopwatch_->Pause();
        }
    }

    // ゲーム結果の判定
    gameResultManager_->Update();

    // メニューでタイトルへ戻る要求があった場合
    if (menuController_->isRequestToExitTitle_) {
        // シーン変更コマンドを追加
        sceneCommandExecutor_.AddCommand(std::make_unique<SceneChangeCommand>("TitleScene", sceneManager_));
        menuController_->isRequestToExitTitle_ = false;
    }

    // ゲームシーンの更新処理
    sceneCommandExecutor_.ExecuteCommand();
}

void GameScene::NextWave() {
    enemyWaveManager_->StartNextWave();
    auto& enemyMap = enemyManager_->GetEnemyMap();
    for (auto& [typeName, enemyList] : enemyMap) {
        for (auto& enemy : enemyList) {
            collisionManager_->RegisterCollider(enemy->GetCollider());
        }
    }
}

void GameScene::Draw()
{
	BaseScene::Draw();
}

void GameScene::Finalize()
{
	BaseScene::Finalize();

	// ゲームシーンの解放処理
}
}
