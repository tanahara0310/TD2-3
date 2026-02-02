#pragma once
#include "Application/Utility/BulletObject/BulletModel.h"
#include "Engine/Collider/SphereCollider.h"

class SmallBullet : public BulletModel {
public:
    SmallBullet();
    ~SmallBullet() override = default;

    void Initialize() override;
    void Update() override;
    void Draw(const CoreEngine::ICamera* camera) override;

    void OnCollisionEnter(CoreEngine::GameObject* other) override;
    CoreEngine::SphereCollider* GetCollider() override { return collider_.get(); }
private:
    std::unique_ptr<CoreEngine::SphereCollider> collider_;

    float lifeTime_;
    float maxLifeTime_;
    float speed_;
    CoreEngine::Vector3 velocity_;
};