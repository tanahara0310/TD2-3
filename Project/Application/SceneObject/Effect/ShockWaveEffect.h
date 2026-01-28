#pragma once
#include "Application/Utility/BulletObject/BulletModel.h"
class ShockWaveEffect final : public BulletModel {
public:
    ShockWaveEffect();
    ~ShockWaveEffect() override = default;
    void Initialize() override;
    void Update() override;
    void Draw(const CoreEngine::ICamera* camera) override;
#ifdef _DEBUG
    const char* GetObjectName() const override { return "ShockWaveEffect"; }
#endif
private:
    float lifeTimer_;
    float speed_ = 0.2f;
    const float maxLifeTime_ = 0.3f;
};