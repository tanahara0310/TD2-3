#pragma once
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include "Collider.h"
#include "CollisionConfig.h"

/// @brief 衝突判定を一括管理するマネージャークラス
/// @note 登録されたすべてのコライダー間の衝突判定を行い、適切なコールバックを実行
namespace CoreEngine
{
class CollisionManager {
public:
   explicit CollisionManager(CollisionConfig* config);
   ~CollisionManager() = default;

   /// @brief コライダーを登録
   /// @param collider 登録するコライダー
   void RegisterCollider(Collider* collider);

   /// @brief 登録されているすべてのコライダー間の衝突判定を実行
   void CheckAllCollisions();

   /// @brief 登録されているコライダーをすべてクリア
   void Clear();

   /// @brief 登録されているすべてのコライダーを取得
   /// @return コライダーのリスト
   const std::vector<Collider*>& GetAllColliders() const { return colliders_; }

   /// @brief トンネリング防止のしきい値を設定（速度がこの値を超えると連続衝突検出を行う）
   /// @param threshold 速度のしきい値（デフォルト: 5.0）
   void SetTunnelingThreshold(float threshold) { tunnelingThreshold_ = threshold; }

   /// @brief トンネリング防止機能の有効/無効を切り替え
   /// @param enabled 有効にする場合 true
   void SetTunnelingPreventionEnabled(bool enabled) { tunnelingPreventionEnabled_ = enabled; }

   struct ColliderPairHash {
      size_t operator()(const std::pair<Collider*, Collider*>& p) const noexcept {
         return reinterpret_cast<size_t>(p.first) ^ reinterpret_cast<size_t>(p.second);
      }
   };

private:
/// @brief 連続衝突検出（Swept AABB）
/// @param a コライダーA
/// @param b コライダーB
/// @return 衝突情報（first: 衝突したか、second: 衝突時刻[0,1]）
std::pair<bool, float> SweptAABBCheck(Collider* a, Collider* b);

/// @brief 連続衝突検出（Swept Sphere）
/// @param a コライダーA（移動する球体）
/// @param b コライダーB
/// @return 衝突情報（first: 衝突したか、second: 衝突時刻[0,1]）
std::pair<bool, float> SweptSphereCheck(Collider* a, Collider* b);

/// @brief 高速移動判定（前フレームからの移動量をチェック）
/// @param collider チェック対象のコライダー
/// @return 高速移動している場合 true
bool IsFastMoving(Collider* collider);

   std::vector<Collider*> colliders_;
   CollisionConfig* config_ = nullptr;

   // 前フレームの衝突ペアを記録（Enter/Stay/Exitの判定用）
   std::unordered_set<std::pair<Collider*, Collider*>, ColliderPairHash> previousCollisions_;

   // 前フレームの位置を記録（トンネリング防止用）
   std::unordered_map<Collider*, Vector3> previousPositions_;

    // トンネリング防止のしきい値（速度がこれを超えると連続衝突検出を行う）
    float tunnelingThreshold_ = 5.0f;

    // トンネリング防止機能の有効/無効フラグ
    bool tunnelingPreventionEnabled_ = false;
};
}
