#include "ResultScene.h"
#include "Engine/Input/KeyboardInput.h"
#include "Scene/SceneManager.h"

void ResultScene::Initialize(EngineSystem* engine)
{
	// 基底クラスの初期化（カメラ、ライト、グリッド等のセットアップ）
	BaseScene::Initialize(engine);
}

void ResultScene::Update()
{
	// 基底クラスの更新
	BaseScene::Update();

	// KeyboardInput を直接取得
	auto keyboard = engine_->GetComponent<KeyboardInput>();
	if (!keyboard) {
		return;
	}

	// Tキーでタイトルシーンへ遷移
	if (keyboard->IsKeyTriggered(DIK_T)) {
		if (sceneManager_) {
			sceneManager_->ChangeScene("TitleScene");
		}
	}
}

void ResultScene::Draw()
{
	// 基底クラスの描画
	BaseScene::Draw();
}

void ResultScene::Finalize()
{
	// 基底クラスの終了処理
	BaseScene::Finalize();
}
