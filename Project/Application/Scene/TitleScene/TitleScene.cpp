#include "TitleScene.h"
#include "EngineSystem.h"
#include "Scene/SceneManager.h"
#include "Engine/Graphics/Render/RenderManager.h"
#include "Engine/Graphics/TextureManager.h"

#include <memory>
#include "Application/Utility/KeyBindConfig.h"
#include "Application/Utility/Command/SceneAllCommand.h"

#include "Application/SceneObject/Player/Player.h"
#include "Application/SceneObject/Ball/Ball.h"
#include "Application/SceneObject/SkyDome/WhiteSkyDome.h"
#include "Application/Utility/MatsumotoUtility.h"

namespace CoreEngine
{
void TitleScene::Initialize(EngineSystem* engine)
{
	BaseScene::Initialize(engine);

	// コンポーネントを直接取得
	auto dxCommon = engine_->GetComponent<DirectXCommon>();
	auto renderManager = engine_->GetComponent<RenderManager>();

	if (!dxCommon || !renderManager) {
		return;
	}

	// タイトルシーンの初期化処理
    sceneCommandExecutor_.Initialize();

    player_ = CreateObject<Player>();
    ball_ = CreateObject<Ball>();
    player_->Initialize();
    ball_->Initialize();
    ballController_ = std::make_unique<BallController>(ball_, player_, this);

    skyDome_ = CreateObject<WhiteSkyDome>();
    skyDome_->SetColor(MatsumotoUtility::ColorEggplant);

}

void TitleScene::OnUpdate()
{
    // 入力処理更新
    KeyBindConfig::Instance().Update();

    // "Start" キーが押されたらゲームシーンへ遷移
    if (KeyBindConfig::Instance().IsTrigger("Start")) {
        // シーン変更コマンドを追加
        sceneCommandExecutor_.AddCommand(std::make_unique<SceneChangeCommand>("GameScene", sceneManager_));
    }

    player_->Update();
    ball_->Update();
    ballController_->Update();

    // タイトルシーンの更新処理
    sceneCommandExecutor_.ExecuteCommand();
}

void TitleScene::Draw()
{
	BaseScene::Draw();

	// タイトルシーンの描画処理
}

void TitleScene::Finalize()
{
	BaseScene::Finalize();

	// タイトルシーンの解放処理
}
}
