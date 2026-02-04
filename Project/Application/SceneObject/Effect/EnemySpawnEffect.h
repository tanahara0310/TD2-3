#pragma once
#include "Application/Utility/BulletObject/BulletModel.h"

class EnemySpawnEffect final : public BulletModel {
public:
    EnemySpawnEffect();
    ~EnemySpawnEffect() override = default;
    void Initialize() override;
    void Update() override;
    void Draw(const CoreEngine::ICamera* camera) override;
#ifdef _DEBUG
    const char* GetObjectName() const override { return "EnemySpawnEffect"; }
#endif
private:
    float lifeTimer_;
    float speed_ = 0.1f;
    const float maxLifeTime_ = 5.0f;
};