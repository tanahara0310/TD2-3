#include "Ball.h"
#include <EngineSystem.h>
#include "Engine/Camera/ICamera.h"

#include "Application/Utility/KeyBindConfig.h"

Ball::Ball() {
    // 必須コンポーネントの取得
    auto engine = GetEngineSystem();

    auto dxCommon = engine->GetComponent<CoreEngine::DirectXCommon>();
    auto modelManager = engine->GetComponent<CoreEngine::ModelManager>();

    if (!dxCommon || !modelManager) {
        return;
    }

    // 静的モデルとして作成
    model_ = modelManager->CreateStaticModel("ApplicationAssets/Model/white1x1Box.obj");
    model_->SetMaterialColor({ 1.0f, 1.0f, 0.0f, 1.0f });

    // トランスフォームの初期化
    transform_.Initialize(dxCommon->GetDevice());

    // テクスチャの読み込み
    auto& textureManager = CoreEngine::TextureManager::GetInstance();
    texture_ = textureManager.Load("Texture/white1x1.png");

    // アクティブ状態に設定
    SetActive(false);
    rotateSpeed_ = 0.1f;

    collider_ = std::make_unique<CoreEngine::SphereCollider>(this, 0.8f);
    collider_->SetLayer(CoreEngine::CollisionLayer::Item);
}

void Ball::Initialize() {
    SetActive(false);
}

void Ball::Update() {
    if (!IsActive() || !model_) {
        return;
    }

    transform_.rotate.y += rotateSpeed_;

    // トランスフォームの更新
    transform_.TransferMatrix();
}

void Ball::Draw(const CoreEngine::ICamera* camera) {
    if (!camera || !model_) return;

    // モデルの描画
    model_->Draw(transform_, camera, texture_.gpuHandle);
}

CoreEngine::Vector3& Ball::GetTransform() {
    return transform_.translate;
}

void Ball::OnCollisionEnter(GameObject* other) {
    (void)other;
    isHitEnemy_ = true;
    hitPos_ = other->GetWorldPosition();
}
