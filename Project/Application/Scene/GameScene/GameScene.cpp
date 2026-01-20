#include "GameScene.h"
#include "EngineSystem.h"
#include "Scene/SceneManager.h"
#include "Engine/Graphics/Render/RenderManager.h"
#include "Engine/Graphics/TextureManager.h"

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

	// ゲームシーンの初期化処理
}

void GameScene::OnUpdate()
{
	// ゲームシーンの更新処理
}

void GameScene::Draw()
{
	BaseScene::Draw();

	// ゲームシーンの描画処理
}

void GameScene::Finalize()
{
	BaseScene::Finalize();

	// ゲームシーンの解放処理
}
}
