#include "WhiteSkyDome.h"
#include <EngineSystem.h>
#include "Engine/Camera/ICamera.h"

WhiteSkyDome::WhiteSkyDome() {
    // 必須コンポーネントの取得
    auto engine = GetEngineSystem();

    auto dxCommon = engine->GetComponent<CoreEngine::DirectXCommon>();
    auto modelManager = engine->GetComponent<CoreEngine::ModelManager>();

    if (!dxCommon || !modelManager) {
        return;
    }

    // 静的モデルとして作成
    model_ = modelManager->CreateStaticModel("ApplicationAssets/Model/skyDome300.obj");
    model_->SetMaterialColor({ 0.0f, 1.0f, 0.0f, 1.0f });
    model_->GetMaterialManager()->SetEnableLighting(false);
    model_->GetMaterialManager()->SetShadingMode(0);

    // トランスフォームの初期化
    transform_.Initialize(dxCommon->GetDevice());

    // テクスチャの読み込み
    auto& textureManager = CoreEngine::TextureManager::GetInstance();
    texture_ = textureManager.Load("Texture/white1x1.png");

    // アクティブ状態に設定
    SetActive(true);
}

void WhiteSkyDome::Initialize() {
}

void WhiteSkyDome::Update() {
    transform_.TransferMatrix();
}

void WhiteSkyDome::Draw(const CoreEngine::ICamera* camera) {
    if (!camera || !model_) return;

    // モデルの描画
    model_->Draw(transform_, camera, texture_.gpuHandle);
}

CoreEngine::Vector3& WhiteSkyDome::GetTransform() {
    return transform_.translate;
}
