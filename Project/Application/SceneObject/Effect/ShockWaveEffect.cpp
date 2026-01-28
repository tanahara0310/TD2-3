#include "ShockWaveEffect.h"
#include "Application/Utility/MatsumotoUtility.h"

ShockWaveEffect::ShockWaveEffect() :
    BulletModel("ApplicationAssets/Model/umbrella.obj", "Texture/white1x1.png"),
    lifeTimer_(0.0f) {
}

void ShockWaveEffect::Initialize() {
    lifeTimer_ = 0.0f;
    speed_ = 0.9f;
    SetActive(false);
    transform_.scale = CoreEngine::Vector3{ 0.3f, 0.3f, 0.3f };

    model_->SetMaterialColor(MatsumotoUtility::ColorYellow);
    model_->GetMaterialManager()->SetEnableLighting(false);
    model_->GetMaterialManager()->SetShadingMode(0);
}

void ShockWaveEffect::Update() {
    if (!IsActive()) {
        return;
    }
    lifeTimer_ += 1.0f / 60.0f; // 仮のデルタタイム
    // エフェクトの拡大
    transform_.scale += CoreEngine::Vector3{ speed_, speed_, speed_ };
    speed_ *= 0.98f;
    // ライフタイマーが最大値を超えたら非アクティブ化
    if (lifeTimer_ >= maxLifeTime_) {
        SetActive(false);
    }

    // 向いている方向に少し動く
    transform_.translate += CoreEngine::Vector3{
       cosf(transform_.rotate.y + 3.14f * 0.5f) * speed_ * speed_ * 0.3f,
        0.0f,
        -sinf(transform_.rotate.y + 3.14f * 0.5f) * speed_ * speed_ * 0.3f
    };
    CoreEngine::Vector4 color = model_->GetMaterialColor();
    model_->SetMaterialColor({ color.x, color.y, color.z, 1.0f - (lifeTimer_ / maxLifeTime_) });

    transform_.TransferMatrix();
}

void ShockWaveEffect::Draw(const CoreEngine::ICamera* camera) {
    if (!camera || !model_ || !IsActive()) return;
    // モデルの描画
    model_->Draw(transform_, camera, texture_.gpuHandle);
}
