#include "IEnemy.h"
#include <EngineSystem.h>
#include "Engine/Camera/ICamera.h"

IEnemy::IEnemy(const std::string& modelPath, const std::string& texturePath) {
    // 必須コンポーネントの取得
    auto engine = GetEngineSystem();

    auto dxCommon = engine->GetComponent<CoreEngine::DirectXCommon>();
    auto modelManager = engine->GetComponent<CoreEngine::ModelManager>();

    if (!dxCommon || !modelManager) {
        return;
    }

    // 静的モデルとして作成
    model_ = modelManager->CreateStaticModel(modelPath);
    model_->SetMaterialColor({ 1.0f, 0.0f, 0.0f, 1.0f });

    // トランスフォームの初期化
    transform_.Initialize(dxCommon->GetDevice());

    // テクスチャの読み込み
    auto& textureManager = CoreEngine::TextureManager::GetInstance();
    texture_ = textureManager.Load(texturePath);

    // アクティブ状態に設定
    SetActive(true);

    collider_ = std::make_unique<CoreEngine::SphereCollider>(this, 0.5f);
    collider_->SetLayer(CoreEngine::CollisionLayer::Enemy);
    
    isAlive_ = true;
}

void IEnemy::Initialize() {
}

void IEnemy::Update() {
    transform_.TransferMatrix();
}

void IEnemy::Draw(const CoreEngine::ICamera* camera) {
    if (!camera || !model_) return;

    // モデルの描画
    model_->Draw(transform_, camera, texture_.gpuHandle);
}