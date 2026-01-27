#pragma once
#include <EngineSystem.h>
#include "Engine/ObjectCommon/GameObject.h"
#include "Engine/Collider/SphereCollider.h"
#include <externals/nlohmann/single_include/nlohmann/json.hpp>

class Player final : public CoreEngine::GameObject {
public:
    Player();

    void Initialize();
    void Update() override;
    void Draw(const CoreEngine::ICamera* camera) override;
#ifdef _DEBUG
    const char* GetObjectName() const override { return "Player"; }
#endif

    CoreEngine::Vector3& GetTransform();
    CoreEngine::Vector3* GetPosPtr() { return &transform_.translate; }
    CoreEngine::SphereCollider* GetCollider() { return collider_.get(); }

    void OnCollisionEnter(GameObject* other) override;

    bool canMove_;
    bool isDamaged_;
    CoreEngine::Vector3 lookDir_;
    
    void PlaySE(const std::string& soundKey);

private:
    std::unique_ptr<CoreEngine::SphereCollider> collider_;

    CoreEngine::Vector3 defaultScale_;
    CoreEngine::Vector3 localScaleAnimValue_;
    CoreEngine::Vector3 velocity_;
    nlohmann::json config_;

    float animTimer_;
    float damageInvincibilityTimer_;

    std::map<std::string, std::unique_ptr<CoreEngine::SoundManager::SoundResource>> soundResources_;
};