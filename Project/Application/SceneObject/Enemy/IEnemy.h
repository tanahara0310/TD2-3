#pragma once
#include "Engine/ObjectCommon/GameObject.h"
#include "Engine/Collider/SphereCollider.h"
#include <externals/nlohmann/single_include/nlohmann/json.hpp>

class IEnemy : public CoreEngine::GameObject {
public:
    IEnemy() = delete;
    explicit IEnemy(const std::string& modelPath, const std::string& texturePath);

    virtual void EnemyUpdate() = 0;

    virtual void Initialize() = 0;
    void Update() override;
    void Draw(const CoreEngine::ICamera* camera) override;
#ifdef _DEBUG
    virtual const char* GetObjectName() const override { return "IEnemy"; }
#endif
    CoreEngine::Vector3& GetTransform() { return transform_.translate; }
    CoreEngine::Vector3* GetPosPtr() { return &transform_.translate; }
    CoreEngine::SphereCollider* GetCollider() { return collider_.get(); }
    virtual void OnCollisionEnter(CoreEngine::GameObject* other) override = 0;
    bool IsAlive() const { return isAlive_; }
    void SetAlive(bool val) { isAlive_ = val; }

    virtual void PlaySE(const std::string& soundKey) = 0;

protected:
    std::unique_ptr<CoreEngine::SphereCollider> collider_;
    bool isAlive_;
};
