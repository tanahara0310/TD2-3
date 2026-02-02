#pragma once
#include <EngineSystem.h>
#include "Engine/ObjectCommon/GameObject.h"
#include "Engine/Collider/SphereCollider.h"
#include <externals/nlohmann/single_include/nlohmann/json.hpp>

enum class PlayerMode {
    YoYo,
    Gun,
};

class Player final : public CoreEngine::GameObject {
public:
    Player();

    void Initialize();
    void Update() override;
    void UpdateTransform();
    void Draw(const CoreEngine::ICamera* camera) override;
#ifdef _DEBUG
    const char* GetObjectName() const override { return "Player"; }
#endif

    CoreEngine::Vector3 GetRotate() const { return transform_.rotate; }
    void SetRotate(const CoreEngine::Vector3& rot) { transform_.rotate = rot; }
    CoreEngine::Vector3& GetTransform();
    CoreEngine::Vector3* GetPosPtr() { return &transform_.translate; }
    CoreEngine::SphereCollider* GetCollider() { return collider_.get(); }

    void OnCollisionEnter(GameObject* other) override;

    // 射撃のクールダウン
    float shootCooldownTimer_;
    float shootCooldownDuration_;

    bool canMove_;
    bool isDamaged_;
    bool shootingBullet_;
    CoreEngine::Vector3 lookDir_;
    
    void PlaySE(const std::string& soundKey);

    void SetInvincibilityTimeAfterWarp(float time) { warpInvincibilityTimer_ = time; }

    void SetDamageInvincibilityTime(float time) { damageInvincibilityTimer_ = time; }
    float GetDamageInvincibilityTime() const { return damageInvincibilityTimer_; }

    CoreEngine::Vector2 GetMoveAxis() const;
    CoreEngine::Vector3 GetLookDir() const;
    CoreEngine::Vector3 GetVelocity() const { return velocity_; }
    void SetVelocity(const CoreEngine::Vector3& vel) { velocity_ = vel; }

    PlayerMode GetPlayerMode() const { return playerMode_; }

private:
    std::unique_ptr<CoreEngine::SphereCollider> collider_;

    CoreEngine::Vector3 defaultScale_;
    CoreEngine::Vector3 localScaleAnimValue_;
    CoreEngine::Vector3 velocity_;
    nlohmann::json config_;

    float animTimer_;
    float damageInvincibilityTimer_;
    // ワープ後の無敵時間タイマー
    float warpInvincibilityTimer_;

    

    PlayerMode playerMode_;

    std::map<std::string, std::unique_ptr<CoreEngine::SoundManager::SoundResource>> soundResources_;
};