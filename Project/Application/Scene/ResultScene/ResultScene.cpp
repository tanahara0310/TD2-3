#include "ResultScene.h"
#include "EngineSystem.h"
#include "Scene/SceneManager.h"
#include "Engine/Graphics/Render/RenderManager.h"
#include "Engine/Graphics/TextureManager.h"

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
}

void ResultScene::OnUpdate()
{
	// リザルトシーンの更新処理
}

void ResultScene::Draw()
{
	BaseScene::Draw();

	// リザルトシーンの描画処理
}

void ResultScene::Finalize()
{
	BaseScene::Finalize();

	// リザルトシーンの解放処理
}
}
