#include "SmallBullet.h"
#include "Application/Utility/MatsumotoUtility.h"

SmallBullet::SmallBullet() :
    BulletModel("ApplicationAssets/Model/sphere1x1.obj", "Texture/white1x1.png"),
    lifeTime_(0.0f),
    maxLifeTime_(5.0f) // 小弾丸の最大寿命を5秒に設定
{
    collider_ = std::make_unique<CoreEngine::SphereCollider>(this, 1.5f);
    collider_->SetLayer(CoreEngine::CollisionLayer::PlayerBullet);
    collider_->SetEnabled(false); // 初期状態では無効
    SetTag("PlayerGunAttack");
}

void SmallBullet::Initialize() {
    lifeTime_ = 0.0f;
    transform_.scale = CoreEngine::Vector3{ 1.0f, 1.0f, 1.0f };
    model_->SetMaterialColor(CoreEngine::Vector4{ 1.0f, 0.8f, 0.0f, 1.0f }); // 黄色っぽい色
    model_->GetMaterialManager()->SetEnableLighting(true);
    model_->GetMaterialManager()->SetShadingMode(1); // スムースシェーディング
    velocity_ = CoreEngine::Vector3{ 0.0f, 0.0f, 0.0f };

    collider_->SetLayer(CoreEngine::CollisionLayer::PlayerBullet);

    speed_ = 60.0f; // 小弾丸の速度を設定
}

void SmallBullet::Update() {
    if (!IsActive()) {
        // 非アクティブな弾のコライダーを無効化
        if (collider_) {
            collider_->SetEnabled(false);
        }
        return;
    }
    
    // アクティブな弾のコライダーを有効化
    if (collider_) {
        collider_->SetEnabled(true);
    }
    
    CoreEngine::Vector3 forward = MatsumotoUtility::EulerAngleToDirection(transform_.rotate);

    // 位置の更新
    transform_.translate += forward * (1.0f / 60.0f) * speed_; // 仮のデルタタイム
    // 寿命の更新
    lifeTime_ += 1.0f / 60.0f; // 仮のデルタタイム
    if (lifeTime_ >= maxLifeTime_) {
        SetActive(false); // 寿命が尽きたら非アクティブ化
        if (collider_) {
            collider_->SetEnabled(false);
        }
    }
    transform_.TransferMatrix();
}

void SmallBullet::Draw(const CoreEngine::ICamera* camera) {
    if (!camera || !model_ || !IsActive()) return;
    // モデルの描画
    model_->Draw(transform_, camera, texture_.gpuHandle);
}

void SmallBullet::OnCollisionEnter(CoreEngine::GameObject* other) {
    if (other->GetTag() == std::string("Enemy")) {
    }
}
