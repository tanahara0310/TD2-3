#include "HitEffect.h"

HitEffect::HitEffect() :
    BulletModel("ApplicationAssets/Model/ring.obj", "Texture/white1x1.png"),
    lifeTimer_(0.0f) {
}

void HitEffect::Initialize() {
    lifeTimer_ = 0.0f;
    speed_ = 0.5f;
    SetActive(false);
    transform_.scale = CoreEngine::Vector3{ 0.3f, 0.3f, 0.3f };
    model_->SetMaterialColor({ 1.0f, 1.0f, 1.0f, 1.0f });
}

void HitEffect::Update() {
    if (!IsActive()) {
        return;
    }
    lifeTimer_ += 1.0f / 60.0f; // 仮のデルタタイム
    // エフェクトの拡大
    transform_.rotate.y += speed_ * 2.0f;
    transform_.scale += CoreEngine::Vector3{ speed_, speed_, speed_ };
    speed_ *= 0.85f;
    // ライフタイマーが最大値を超えたら非アクティブ化
    if (lifeTimer_ >= maxLifeTime_) {
        SetActive(false);
    }

    model_->SetMaterialColor({ 1.0f, 1.0f, 1.0f, 1.0f - (lifeTimer_ / maxLifeTime_) });

    transform_.TransferMatrix();
}

void HitEffect::Draw(const CoreEngine::ICamera* camera) {
    if (!camera || !model_ || !IsActive()) return;
    // モデルの描画
    model_->Draw(transform_, camera, texture_.gpuHandle);
}
