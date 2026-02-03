#pragma once
#include <memory>
#include <EngineSystem.h>
#include "Engine/ObjectCommon/GameObject.h"
#include "Engine/Collider/SphereCollider.h"
#include <externals/nlohmann/single_include/nlohmann/json.hpp>
#include "Application/SceneObject/Bullet/BulletState.h"
#include <stack>

class Ball final : public CoreEngine::GameObject {
public:
    Ball();
    void Initialize();
    void Update() override;
    void Draw(const CoreEngine::ICamera* camera) override;
#ifdef _DEBUG
    const char* GetObjectName() const override { return "Ball"; }
    bool DrawImGuiExtended() override;
#endif
    CoreEngine::Vector3& GetTransform();
    CoreEngine::Vector3* GetPosPtr() { return &transform_.translate; }
    CoreEngine::SphereCollider* GetCollider() { return collider_.get(); }
    void UpdateTransform() { transform_.TransferMatrix(); }

    void OnCollisionEnter(GameObject* other) override;

    void SetConfig(const nlohmann::json& config) override;
    nlohmann::json GetConfig() const override;
    CoreEngine::Vector3 GetMoveDir() const { return moveDir; }

    void SetVelocity(const CoreEngine::Vector3& velocity) { velocity_ = velocity; }
    const CoreEngine::Vector3& GetVelocity() const { return velocity_; }

    void PlaySE(const std::string& soundKey);

    float rotateSpeed_;
    bool isHitEnemy_;
    CoreEngine::Vector3 hitPos_;

    int bulletCount_;
    int maxBulletCount_;

    BulletState PopBulletState();
    std::vector<BulletState> GetAllBulletStates() const;

private:
    std::unique_ptr<CoreEngine::SphereCollider> collider_;

    float speed_;
    CoreEngine::Vector3 direction_;
    CoreEngine::Vector3 velocity_;

    CoreEngine::Vector3 oldPosition_;
    CoreEngine::Vector3 moveDir;

    std::stack<BulletState> bulletStateStack_;
    std::map<std::string, std::unique_ptr<CoreEngine::SoundManager::SoundResource>> soundResources_;
};