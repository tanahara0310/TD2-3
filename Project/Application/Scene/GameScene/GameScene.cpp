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
#include "Application/SceneObject/BackGround/Ground.h"
#include "Application/SceneObject/Enemy/AllEnemy.h"
#include "Application/SceneObject/SkyDome/WhiteSkyDome.h"

#include "Application/Utility/Command/SceneAllCommand.h"

#include "Application/SceneObject/Effect/AllEffect.h"

#include "Application/Utility/ApplicationGlobalValue.h"

#include "Application/SceneObject/Bullet/AllBullet.h"

#include "Application/SceneObject/Score/ScoreCounter.h"

namespace CoreEngine
{
    void GameScene::Initialize(EngineSystem* engine) {
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
        // メニューコントローラーの生成
        menuController_ = std::make_unique<MenuController>(sceneCommandExecutor_);
        menuController_->Initialize();

        cameraController_ = std::make_unique<CameraController>(cameraManager_.get());
        CoreEngine::Camera* camera =
            static_cast<CoreEngine::Camera*>(cameraManager_->GetActiveCamera(CoreEngine::CameraType::Camera3D));
        camera->SetTranslate({ 0.0f, 30.0f, 0.0f });
        camera->SetRotate({ 3.14f * 0.4f,0.0f,0.0f });
        CameraParameters params = camera->GetParameters();
        params.fov = MatsumotoUtility::DegreesToRadians(40.0f);
        camera->SetParameters(params);
        cameraController_->Initialize();

        gameStopwatch_ = std::make_unique<Stopwatch>();
        gameStopwatch_->Start();

        // エフェクトの生成
        effectContainers_.clear();
        effectContainers_["HitEffect"] = std::make_unique<BulletObjectContainer>(20);
        effectContainers_["HitEffect"]->ApplyToScene<HitEffect>(this);
        effectContainers_["ShockWaveEffect"] = std::make_unique<BulletObjectContainer>(20);
        effectContainers_["ShockWaveEffect"]->ApplyToScene<ShockWaveEffect>(this);
        effectContainers_["SlashEffect"] = std::make_unique<BulletObjectContainer>(20);
        effectContainers_["SlashEffect"]->ApplyToScene<SlashEffect>(this);
        effectContainers_["PlayerBullet"] = std::make_unique<BulletObjectContainer>(50);
        effectContainers_["PlayerBullet"]->ApplyToScene<SmallBullet>(this);

        // ゲームオブジェクトの生成
        player_ = CreateObject<Player>();
        player_->SetAutoUpdate(false);
        ball_ = CreateObject<Ball>();
        ball_->SetAutoUpdate(false);
        ground_ = CreateObject<Ground>();
        ballController_ = std::make_unique<BallController>(ball_, player_, this);
        screenUI_ = std::make_unique<ScreenUI>(
            this, player_, gameStopwatch_.get(), ballController_.get(), menuController_.get());
        screenUI_->Initialize();

        //cameraController_->SetCameraWork<FollowCamera>(player_->GetTransform(), CoreEngine::Vector3(0.0f, 50.0f, -14.0f), 0.1f);
        cameraController_->SetDefaultCameraWork<FollowCamera>(
            player_->GetTransform(), CoreEngine::Vector3(-7.5f, 60.0f, -21.0f), 0.1f);
        cameraController_->ResetDefaultCameraWork();

        skyDome_ = CreateObject<WhiteSkyDome>();
        skyDome_->SetColor(MatsumotoUtility::ColorYellow);

        // プレイヤーの初期化
        player_->Initialize();
        ball_->Initialize();

        enemyManager_ = std::make_unique<EnemyContainer>(this);

        // ボールコントローラーの生成
        ballController_->Initialize();
        ballController_->SetHitEffectFunction(
            std::bind(&BulletObjectContainer::Spawn,
                effectContainers_["ShockWaveEffect"].get(),
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
        );
        ballController_->SetSlashEffectFunction(
            std::bind(&BulletObjectContainer::Spawn,
                effectContainers_["SlashEffect"].get(),
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
        );

        // 敵配置データのロード
        enemyMapLoader_ = std::make_unique<EnemyMapLoader>(enemyManager_.get(), player_);

        enemyMapLoader_->LoadEnemyMap("testD.json");
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
            ballController_.get(),
            gameStopwatch_.get());
        enemyKillMotionManager_->isPlayingMotion_ = false;
        // キルエフェクト関数の設定
        enemyKillMotionManager_->SetKillEffectFunction(
            std::bind(&BulletObjectContainer::Spawn,
                effectContainers_["HitEffect"].get(),
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
        );

        // 敵ウェーブマネージャーの生成
        enemyWaveManager_ = std::make_unique<EnemyWaveManager>(enemyMapLoader_.get());

        collisionConfig_ = std::make_unique<CollisionConfig>();
        collisionManager_ = std::make_unique<CollisionManager>(collisionConfig_.get());

        collisionConfig_->SetCollisionEnabled(CollisionLayer::Player, CollisionLayer::Enemy, true);
        collisionConfig_->SetCollisionEnabled(CollisionLayer::Enemy, CollisionLayer::Item, true);
        collisionConfig_->SetCollisionEnabled(CollisionLayer::PlayerBullet, CollisionLayer::Enemy, true);

        collisionManager_->Clear();
        collisionManager_->RegisterCollider(player_->GetCollider());
        collisionManager_->RegisterCollider(ball_->GetCollider());
        auto& enemyMap = enemyManager_->GetEnemyMap();
        for (auto& [typeName, enemyList] : enemyMap) {
            for (auto& enemy : enemyList) {
                collisionManager_->RegisterCollider(enemy->GetCollider());
            }
        }

        auto& bulletContainer = effectContainers_["PlayerBullet"];
        for (auto& bullet : bulletContainer->GetBulletObjects()) {
            assert(bullet->GetCollider());
            collisionManager_->RegisterCollider(bullet->GetCollider());
        }

        // メニューViewの生成
        menuView_ = std::make_unique<MenuView>(this, menuController_.get());
        menuView_->Initialize();

        // ゲーム結果マネージャーの生成
        gameResultManager_ = std::make_unique<GameResultManager>();
        gameResultManager_->Initialize();
        // 時間経過&&死んでる敵がしっかり非アクティブでクリア
        std::function<bool()> timeUpCondition = [this]() {
            if (enemyManager_->DeathEnemyList().size() > 0) {
                return false;
            }

            bool isEnd = gameStopwatch_->ElapsedMilliseconds() >= ApplicationGlobalValue::GAME_CLEAR_TIME_MS;
            //if (!isSpawnBoss_ && isEnd && ScoreCounter::GetInstance().GetScore() >= 0) {
            //    isSpawnBoss_ = true;
            //    gameStopwatch_->Start(); // ボス戦用に時間をリセットして再スタート
            //    enemyManager_->SpawnEnemy<BossEnemy>(CoreEngine::Vector3(0.0f, 0.0f, 0.0f));
            //    return false;
            //}
            return isEnd;
            };
        gameResultManager_->AddGameClearCondition(timeUpCondition);

        // ゲームルールの設定
        gameRule_ = std::make_unique<TimeAndEnemyCountSpawnRule>(
            std::bind(&GameScene::NextWave, this),
            std::bind(&EnemyContainer::GetAliveEnemyCount, enemyManager_.get()));

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

        // 謎の音声リソース読み込めないバグ
        shotSoundResources_
            = soundManager->CreateSoundResource("ApplicationAssets/Sound/SE_BulletShot.mp3");
    }

    void GameScene::OnUpdate() {
        KeyBindConfig& keyBindConfig = KeyBindConfig::Instance();
#ifdef _DEBUG
        ImGui::Begin("Game Controller");
        ImGui::Text("Score : %d", ScoreCounter::GetInstance().GetScore());

        // 時間の表示
        ImGui::Text("Elapsed Time: %.2f ms", gameStopwatch_->ElapsedMilliseconds());
        // バーで表示
        float timeRatio = static_cast<float>(gameStopwatch_->ElapsedMilliseconds() / ApplicationGlobalValue::GAME_CLEAR_TIME_MS);
        ImGui::ProgressBar(timeRatio, ImVec2(0.0f, 0.0f), "Time to Clear");
        ImGui::End();
#endif

        // 入力処理更新
        keyBindConfig.Update();

        if (gameResultManager_->CheckGameClear()) {
            gameClearSequence_->Update();
            sceneCommandExecutor_.ExecuteCommand();
            return;
        }

        // メニューコントローラーの更新
        menuController_->Update();
        menuView_->Update();


        // メニューが閉じている場合のみゲームシーンを更新
        if (!menuController_->IsMenuOpen()) {
            if (!gameStopwatch_->IsRunning()) {
                gameStopwatch_->Resume();
            }

            cameraController_->Update();
            if (!enemyKillMotionManager_->isPlayingMotion_) {
                player_->Update();
                ball_->Update();

#pragma region PlayerLockOn
                bool canShoot = !ball_->IsActive() && player_->GetPlayerMode() == PlayerMode::Gun;
                //if (canShoot) {
                //    // ロックオン処理
                //    auto enemies = enemyManager_->GetAliveEnemies();
                //    // プレイヤーの向きと最も近い敵を探す（向き優先、次に距離）
                //    IEnemy* bestEnemy = nullptr;
                //    float bestScore = -1.0f; // コサイン類似度の最大値を探す
                //    float bestDistance = 999999.9f;

                //    const CoreEngine::Vector3 playerPos = player_->GetWorldPosition();
                //    CoreEngine::Vector3 playerDir = CoreEngine::Math::Vector::Normalize(-player_->lookDir_);

                //    for (auto& enemy : enemies) {
                //        const CoreEngine::Vector3 enemyPos = enemy->GetWorldPosition();
                //        const CoreEngine::Vector3 toEnemy = CoreEngine::Math::Vector::Normalize(enemyPos - playerPos);
                //        float dot = CoreEngine::Math::Vector::Dot(playerDir, toEnemy); // -1.0〜1.0

                //        // 距離も考慮（正面優先、同じなら近い方）
                //        float distance = CoreEngine::Math::Vector::Length(enemyPos - playerPos);

                //        // ある程度正面（15度以内）だけを対象
                //        if (dot > 0.9659f) { // 15度以内
                //            if (dot > bestScore || (dot == bestScore && distance < bestDistance)) {
                //                bestScore = dot;
                //                bestDistance = distance;
                //                bestEnemy = enemy;
                //            }
                //        }
                //    }

                //    if (bestEnemy) {
                //        CoreEngine::Vector3 direction = CoreEngine::Math::Vector::Normalize(
                //            bestEnemy->GetWorldPosition() - player_->GetWorldPosition());
                //        player_->lookDir_ = direction;
                //        // プレイヤーの向きを更新
                //        float targetYaw = atan2f(direction.x, direction.z);
                //        CoreEngine::Vector3 playerRotate = player_->GetRotate();
                //        playerRotate.y = targetYaw;
                //        player_->SetRotate(playerRotate);
                //        player_->UpdateTransform();
                //    }
                //}
#pragma endregion
            // 謎のイテレータ破壊が起きるので、プレイヤーが弾を打つ処理をここで行う(型依存によるSTLイテレータデバッグ機構のバグの可能性)
#pragma region PlayerBulletShot
            // ボールがアクティブじゃなくて、プレイヤーがガンモードのときに弾を発射
                player_->shootingBullet_ = false;
                if (keyBindConfig.IsTrigger("Shot") && canShoot && ball_->bulletCount_ > 0) {
                    if (player_->shootCooldownTimer_ <= 0.0f) {
                        player_->shootCooldownTimer_ = player_->shootCooldownDuration_;
                        Vector3 shotRotate = player_->GetRotate();
                        shotRotate.y -= 3.14f * 0.5f;

                        // 弾オブジェクトをコンテナから取得して初期化
                        int result =
                            effectContainers_["PlayerBullet"]->Spawn(
                                player_->GetWorldPosition(),
                                shotRotate,
                                CoreEngine::Vector3(0.1f, 0.1f, 0.1f));
                        result;
                        ball_->bulletCount_--;
                        player_->shootingBullet_ = true;

                        // プレイヤーの目の前から向いている向きに発生
                        CoreEngine::Vector3 direction = CoreEngine::Math::Vector::Normalize(player_->lookDir_);
                        direction.x *= -1.0f;
                        // directionを90度回転させる
                        direction = CoreEngine::Math::Vector::Normalize(
                            CoreEngine::Math::Vector::Cross(direction, CoreEngine::Vector3(0.0f, -1.0f, 0.0f)));

                        effectContainers_["SlashEffect"]->Spawn(
                            player_->GetWorldPosition() + CoreEngine::Math::Vector::Normalize(direction) * 25.0f,
                            MatsumotoUtility::DirectionToEulerAngle(direction),
                            CoreEngine::Vector3(20.0f, 0.1f, 50.0f));

                        player_->SetVelocity(
                            -CoreEngine::Math::Vector::Normalize(direction * 0.5f));

                        shotSoundResources_->Play(false);

                        // 発射音などの効果音を再生する場合はここで行う
                        /*if (result != -1) {
                            player_->PlaySE("gun_shot");
                        }*/
                    }
                }
#pragma endregion

                ballController_->Update();
                gameRule_->Update();
                enemyManager_->Update();
            }


            enemyKillComboCounter_->Update();
            enemyKillMotionManager_->Update();

            screenUI_->Update();

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

    void GameScene::Draw() {
        BaseScene::Draw();
    }

    void GameScene::Finalize() {
        BaseScene::Finalize();

        // ゲームシーンの解放処理
    }
}
