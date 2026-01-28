#pragma once
#include "Application/Utility/BulletObject/BulletModel.h"

class HitEffect final : public BulletModel {
public:
    HitEffect();
    ~HitEffect() override = default;
    void Initialize() override;
    void Update() override;
    void Draw(const CoreEngine::ICamera* camera) override;
#ifdef _DEBUG
    const char* GetObjectName() const override { return "HitEffect"; }
#endif
private:
    float lifeTimer_;
    float speed_ = 0.1f;
    const float maxLifeTime_ = 0.5f;
};