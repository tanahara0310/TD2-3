#pragma once
#include <memory>
#include <EngineSystem.h>
#include "Engine/ObjectCommon/GameObject.h"
#include "Engine/Collider/SphereCollider.h"
#include <externals/nlohmann/single_include/nlohmann/json.hpp>

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

    void OnCollisionEnter(GameObject* other) override;

    void SetConfig(const nlohmann::json& config) override;
    nlohmann::json GetConfig() const override;

    void PlaySE(const std::string& soundKey);

    float rotateSpeed_;
    bool isHitEnemy_;
    CoreEngine::Vector3 hitPos_;

private:
    std::unique_ptr<CoreEngine::SphereCollider> collider_;

    float speed_;
    CoreEngine::Vector3 direction_;

    std::map<std::string, std::unique_ptr<CoreEngine::SoundManager::SoundResource>> soundResources_;
};