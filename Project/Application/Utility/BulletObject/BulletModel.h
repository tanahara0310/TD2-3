#pragma once
#include <string>
#include "Engine/ObjectCommon/GameObject.h"
#include "Engine/Collider/SphereCollider.h"

/// @brief 複数の同じモデルを扱うための基底クラス
class BulletModel : public CoreEngine::GameObject {
public:
    BulletModel() = delete;
    explicit BulletModel(const std::string& modelPath, const std::string& texturePath);
    ~BulletModel() override = default;

    virtual void Initialize() = 0;
    virtual void Update() = 0;
    virtual void Draw(const CoreEngine::ICamera* camera) = 0;

    virtual CoreEngine::SphereCollider* GetCollider() { return nullptr; };

    CoreEngine::WorldTransform& GetTransform() { return transform_; }
};