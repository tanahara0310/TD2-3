#include "TitleScene.h"
#include "EngineSystem.h"
#include "Scene/SceneManager.h"
#include "Engine/Graphics/Render/RenderManager.h"
#include "Engine/Graphics/TextureManager.h"

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
}

void TitleScene::OnUpdate()
{
	// タイトルシーンの更新処理
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
