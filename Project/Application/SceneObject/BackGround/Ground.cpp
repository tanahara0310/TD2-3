#include "Ground.h"
#include "Engine/Camera/ICamera.h"
#include "Application/Utility/MatsumotoUtility.h"

Ground::Ground() {
    // 必須コンポーネントの取得
    auto engine = GetEngineSystem();

    auto dxCommon = engine->GetComponent<CoreEngine::DirectXCommon>();
    auto modelManager = engine->GetComponent<CoreEngine::ModelManager>();

    if (!dxCommon || !modelManager) {
        return;
    }

    // 静的モデルとして作成
    model_ = modelManager->CreateStaticModel("ApplicationAssets/Model/disk.obj");
    model_->SetMaterialColor(MatsumotoUtility::ColorBrass);

    // トランスフォームの初期化
    transform_.Initialize(dxCommon->GetDevice());

    // テクスチャの読み込み
    auto& textureManager = CoreEngine::TextureManager::GetInstance();
    texture_ = textureManager.Load("Texture/white1x1.png");

    // アクティブ状態に設定
    SetActive(true);

    float radius = 50.0f;
    transform_.scale = { radius, 1.0f, radius };
    transform_.translate.y = -1.0f;
}

void Ground::Initialize() {
}

void Ground::Update() {
    transform_.TransferMatrix();
}

void Ground::Draw(const CoreEngine::ICamera* camera) {
    if (!camera || !model_) return;

    // モデルの描画
    model_->Draw(transform_, camera, texture_.gpuHandle);
}
