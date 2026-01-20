#include "GameScene.h"
#include "EngineSystem.h"
#include "Scene/SceneManager.h"
#include "Engine/Graphics/Render/RenderManager.h"
#include "Engine/Graphics/TextureManager.h"

// アプリケーションのユーティリティ
#include "Application/Utility/KeyBindConfig.h"
#include "Application/SceneObject/CameraController/AllCameraWork.h"
// アプリケーションのシーンオブジェクト
#include "Application/SceneObject/Player/Player.h"
#include "Application/SceneObject/Ball/Ball.h"
#include "Application/SceneObject/Enemy/AllEnemy.h"


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
    cameraController_.Initialize();
    menuController_.Initialize();

    // ゲームオブジェクトの生成
    player_ = CreateObject<Player>();
    ball_ = CreateObject<Ball>();

    // プレイヤーの初期化
    player_->Initialize();
    ball_->Initialize();

    enemyManager_ = std::make_unique<EnemyManager>(this);
    enemyManager_->SpawnEnemy<DummyEnemy>({ 3.0f, 0.0f, 0.0f });

    // ボールコントローラーの生成
    ballController_ = std::make_unique<BallController>(ball_, player_);

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
    
}

void GameScene::OnUpdate()
{
    // 入力処理更新
    KeyBindConfig::Instance().Update();

    // メニューコントローラーの更新
    menuController_.Update();

    // メニューが閉じている場合のみゲームシーンを更新
    if (!menuController_.IsMenuOpen()) {
        cameraController_.Update();
        player_->Update();
        ball_->Update();
        ballController_->Update();
        enemyManager_->Update();
        collisionManager_->CheckAllCollisions();
    }

    // ゲームシーンの更新処理
    sceneCommandExecutor_.ExecuteCommand();
}

void GameScene::Draw()
{
	BaseScene::Draw();

	// ゲームシーンの描画処理
    if (menuController_.IsMenuOpen()) {
#ifdef _DEBUG
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse;
        ImGui::Begin("Menu", nullptr, flags);
        ImGui::Text("メニューが開いています");
        ImGui::End();
#endif // _DEBUG
    }
}

void GameScene::Finalize()
{
	BaseScene::Finalize();

	// ゲームシーンの解放処理
}
}
