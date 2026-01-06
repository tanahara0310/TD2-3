#pragma once

#include "Engine/ObjectCommon/GameObject.h"
#include "Engine/Utility/Timer/GameTimer.h"
#include <memory>

class AABBCollider;

/// @brief ジャンプ攻撃の着地判定用一時的なヒットボックス
class JumpAttackHitbox : public GameObject {
public:
    JumpAttackHitbox();
    ~JumpAttackHitbox() override;

    /// @brief 初期化
    /// @param position 着地予定位置
    /// @param size ヒットボックスのサイズ
    /// @param activeDuration アクティブである時間（秒）
    void Initialize(const Vector3& position, const Vector3& size, float activeDuration);

    /// @brief 更新
    void Update() override;

    /// @brief 衝突開始イベント
    void OnCollisionEnter(GameObject* other) override;

    /// @brief コライダーを取得
    AABBCollider* GetCollider() const { return collider_.get(); }

private:
    std::unique_ptr<AABBCollider> collider_;
    GameTimer activeTimer_;
};
