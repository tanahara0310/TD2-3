#include "SlashEffect.h"
#include "Application/Utility/MatsumotoUtility.h"

SlashEffect::SlashEffect() :
    BulletModel("ApplicationAssets/Model/white1x1Box.obj", "Texture/white1x1.png"),
    lifeTimer_(0.0f) {
}

void SlashEffect::Initialize() {
    lifeTimer_ = 0.0f;
    speed_ = 0.5f;
    SetActive(false);
    transform_.scale = CoreEngine::Vector3{ 0.5f, 1.0f, 50.0f };
}

void SlashEffect::Update() {

    if (!IsActive()) {
        return;
    }
    lifeTimer_ += 1.0f / 60.0f; // 仮のデルタタイム
    // エフェクトの拡大
    transform_.scale.x = MatsumotoUtility::SimpleEaseIn(transform_.scale.x, 0.0f, speed_);

    speed_ *= 0.85f;
    // ライフタイマーが最大値を超えたら非アクティブ化
    if (lifeTimer_ >= maxLifeTime_) {
        SetActive(false);
    }
    transform_.TransferMatrix();
}

void SlashEffect::Draw(const CoreEngine::ICamera* camera) {
    if (!camera || !model_ || !IsActive()) return;
    // モデルの描画
    model_->Draw(transform_, camera, texture_.gpuHandle);
}


