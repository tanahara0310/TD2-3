#pragma once
#include "Application/SceneObjects/BulletObject/BulletModel.h"

/// @brief プレイヤーの相棒オブジェクト
class Companion final : public BulletModel {
public:
    /// @brief コンストラクタ
    Companion(
        const std::string& modelPath,
        const std::string& texPath,
        float distance,
        Vector3* targetPos);
    void Initialize() override;
    void Update() override;
    void Draw(const ICamera* camera) override;

    void SetNewTargetPosition(Vector3* newTargetPos) { targetPosition_ = newTargetPos; }

#ifdef _DEBUG
    const char* GetObjectName() const override { return "Companion"; }
#endif // DEBUG

    Vector3& GetTransform();
    Vector3* GetPosPtr() { return &transform_.translate; }
    Vector3* GetTargetPosition() { return targetPosition_; }

private:
    float followDistance_;
    Vector3* targetPosition_;
    float speed_;
};