#include "ResultScene.h"
#include "EngineSystem.h"
#include "Scene/SceneManager.h"
#include "Engine/Graphics/Render/RenderManager.h"
#include "Engine/Graphics/TextureManager.h"

#include "Application/Utility/KeyBindConfig.h"

namespace CoreEngine
{
void ResultScene::Initialize(EngineSystem* engine)
{
	BaseScene::Initialize(engine);

	// コンポーネントを直接取得
	auto dxCommon = engine_->GetComponent<DirectXCommon>();
	auto renderManager = engine_->GetComponent<RenderManager>();

	if (!dxCommon || !renderManager) {
		return;
	}

	// リザルトシーンの初期化処理
    sceneCommandExecutor_.Initialize();
}

void ResultScene::OnUpdate()
{
    // 入力処理更新
    KeyBindConfig::Instance().Update();
}

void ResultScene::Draw()
{
	BaseScene::Draw();

	// リザルトシーンの描画処理
    sceneCommandExecutor_.ExecuteCommand();
}

void ResultScene::Finalize()
{
	BaseScene::Finalize();

	// リザルトシーンの解放処理
}
}
