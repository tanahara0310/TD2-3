#include "EnemySpawnEffect.h"
#include "Application/Utility/MatsumotoUtility.h"

EnemySpawnEffect::EnemySpawnEffect() :
    BulletModel("ApplicationAssets/Model/ring.obj", "Texture/white1x1.png"),
    lifeTimer_(0.0f) {
    
}

void EnemySpawnEffect::Initialize() {
    lifeTimer_ = 0.0f;
    speed_ = 0.5f;
    SetActive(false);
    transform_.scale = CoreEngine::Vector3{ 1.5f, 1.5f, 1.5f };
    model_->SetMaterialColor(MatsumotoUtility::ColorCodeToVector4("#880000"));
    model_->GetMaterialManager()->SetEnableLighting(false);
    model_->GetMaterialManager()->SetShadingMode(0);
}

void EnemySpawnEffect::Update() {
    if (!IsActive()) {
        return;
    }
    lifeTimer_ += 1.0f / 60.0f; // 仮のデルタタイム

    // エフェクトの拡大
    transform_.rotate.y += speed_ * 2.0f;
    transform_.scale = CoreEngine::Vector3{ 
        1.5f + sinf(lifeTimer_) * 0.3f, 1.5f + sinf(lifeTimer_) * 0.3f, 1.5f + sinf(lifeTimer_) * 0.3f };

    // ライフタイマーが最大値を超えたら非アクティブ化
    if (lifeTimer_ >= maxLifeTime_) {
        SetActive(false);
    }

    transform_.TransferMatrix();
}

void EnemySpawnEffect::Draw(const CoreEngine::ICamera* camera) {
    if (!camera || !model_ || !IsActive()) return;
    // モデルの描画
    model_->Draw(transform_, camera, texture_.gpuHandle);
}
