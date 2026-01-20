#include "TitleScene.h"
#include "EngineSystem.h"
#include "Scene/SceneManager.h"
#include "Engine/Graphics/Render/RenderManager.h"
#include "Engine/Graphics/TextureManager.h"

#include <memory>
#include "Application/Utility/KeyBindConfig.h"
#include "Application/Utility/Command/SceneAllCommand.h"

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
