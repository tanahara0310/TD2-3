#include "Companion.h"
#include "Engine/Math/MathCore.h"    

Companion::Companion(
    const std::string& modelPath, 
    const std::string& texPath, 
    float distance,
    Vector3* targetPos): 
    BulletModel(modelPath, texPath),
    followDistance_(distance),
    targetPosition_(targetPos) {}

void Companion::Initialize() {
    isActive_ = false;
    transform_.scale = { 0.5f, 0.5f, 0.5f };
    model_->SetMaterialColor({ 1.0f, 1.0f, 0.0f, 1.0f });
    speed_ = 0.1f;
}

void Companion::Update() {
    if (MathCore::Vector::Length(transform_.GetWorldPosition() - *(targetPosition_)) > followDistance_) {
        // ターゲット位置へのベクトルを計算
        Vector3 toTarget = MathCore::Vector::Subtract(*(targetPosition_), transform_.GetWorldPosition());
        toTarget = MathCore::Vector::Normalize(toTarget);
        // 一定速度でターゲットに近づく
        Vector3 movement = MathCore::Vector::Multiply(speed_, toTarget);
        transform_.translate = MathCore::Vector::Add(transform_.translate, movement);
    }

    // トランスフォームの更新
    transform_.TransferMatrix();
}

void Companion::Draw(const ICamera* camera) {
    if (!camera || !model_) return;

    // モデルの描画
    model_->Draw(transform_, camera, texture_.gpuHandle);
}

Vector3& Companion::GetTransform() {
    return transform_.translate;
}
