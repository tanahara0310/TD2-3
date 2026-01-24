#include "CollisionManager.h"
#include "AABBCollider.h"
#include "SphereCollider.h"
#include <algorithm>
#include <cmath>


namespace CoreEngine
{
namespace {
   // コライダーペアを一意にするためのヘルパー関数
   std::pair<Collider*, Collider*> MakePair(Collider* a, Collider* b) {
      return (a < b) ? std::make_pair(a, b) : std::make_pair(b, a);
   }
}

CollisionManager::CollisionManager(CollisionConfig* config)
   : config_(config) {
}

void CollisionManager::RegisterCollider(Collider* collider) {
   if (collider) colliders_.push_back(collider);
}

bool CollisionManager::IsFastMoving(Collider* collider) {
   if (!tunnelingPreventionEnabled_) return false;

   auto it = previousPositions_.find(collider);
   if (it == previousPositions_.end()) {
      return false;
   }

   Vector3 currentPos = collider->GetPosition();
   Vector3 prevPos = it->second;
   Vector3 delta = currentPos - prevPos;
   float distance = std::sqrt(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);

   return distance > tunnelingThreshold_;
}

std::pair<bool, float> CollisionManager::SweptAABBCheck(Collider* a, Collider* b) {
   if (a->GetType() != ColliderType::AABB || b->GetType() != ColliderType::AABB) {
      return { false, 0.0f };
   }

   AABBCollider* aabbA = static_cast<AABBCollider*>(a);
   AABBCollider* aabbB = static_cast<AABBCollider*>(b);

   auto prevPosIt = previousPositions_.find(a);
   if (prevPosIt == previousPositions_.end()) {
      return { false, 0.0f };
   }

   Vector3 prevPos = prevPosIt->second;
   Vector3 currentPos = a->GetPosition();
   Vector3 velocity = currentPos - prevPos;

   Vector3 posB = b->GetPosition();
   Vector3 sizeA = aabbA->GetSize();
   Vector3 sizeB = aabbB->GetSize();

   Vector3 expandedMin = posB - (sizeA + sizeB) * 0.5f;
   Vector3 expandedMax = posB + (sizeA + sizeB) * 0.5f;

   float tEntry[3], tExit[3];
   
   for (int i = 0; i < 3; ++i) {
      float v = 0.0f;
      float pos = 0.0f;
      
      if (i == 0) { v = velocity.x; pos = prevPos.x; }
      else if (i == 1) { v = velocity.y; pos = prevPos.y; }
      else { v = velocity.z; pos = prevPos.z; }

      float min = (i == 0) ? expandedMin.x : (i == 1) ? expandedMin.y : expandedMin.z;
      float max = (i == 0) ? expandedMax.x : (i == 1) ? expandedMax.y : expandedMax.z;

      if (std::abs(v) < 1e-6f) {
         if (pos < min || pos > max) {
            return { false, 0.0f };
         }
         tEntry[i] = -std::numeric_limits<float>::infinity();
         tExit[i] = std::numeric_limits<float>::infinity();
      } else {
         tEntry[i] = (min - pos) / v;
         tExit[i] = (max - pos) / v;
         
         if (tEntry[i] > tExit[i]) {
            std::swap(tEntry[i], tExit[i]);
         }
      }
   }

   float entryTime = std::max({ tEntry[0], tEntry[1], tEntry[2] });
   float exitTime = std::min({ tExit[0], tExit[1], tExit[2] });

   if (entryTime > exitTime || entryTime > 1.0f || exitTime < 0.0f) {
      return { false, 0.0f };
   }

   return { true, std::max(0.0f, entryTime) };
}

std::pair<bool, float> CollisionManager::SweptSphereCheck(Collider* a, Collider* b) {
   auto prevPosIt = previousPositions_.find(a);
   if (prevPosIt == previousPositions_.end()) {
      return { false, 0.0f };
   }

   Vector3 prevPos = prevPosIt->second;
   Vector3 currentPos = a->GetPosition();
   Vector3 velocity = currentPos - prevPos;
   Vector3 posB = b->GetPosition();

   // Sphere vs Sphere
   if (a->GetType() == ColliderType::Sphere && b->GetType() == ColliderType::Sphere) {
      SphereCollider* sphereA = static_cast<SphereCollider*>(a);
      SphereCollider* sphereB = static_cast<SphereCollider*>(b);

      float radiusSum = sphereA->GetRadius() + sphereB->GetRadius();
      Vector3 relativePos = prevPos - posB;

      // 2次方程式の係数を計算: ||P + t*V - Q||^2 = r^2
      float a_coeff = velocity.x * velocity.x + velocity.y * velocity.y + velocity.z * velocity.z;
      float b_coeff = 2.0f * (relativePos.x * velocity.x + relativePos.y * velocity.y + relativePos.z * velocity.z);
      float c_coeff = relativePos.x * relativePos.x + relativePos.y * relativePos.y + 
                      relativePos.z * relativePos.z - radiusSum * radiusSum;

      float discriminant = b_coeff * b_coeff - 4.0f * a_coeff * c_coeff;

      if (discriminant < 0.0f || std::abs(a_coeff) < 1e-6f) {
         return { false, 0.0f };
      }

      float t = (-b_coeff - std::sqrt(discriminant)) / (2.0f * a_coeff);

      if (t >= 0.0f && t <= 1.0f) {
         return { true, t };
      }

      return { false, 0.0f };
   }

   // Sphere vs AABB
   if (a->GetType() == ColliderType::Sphere && b->GetType() == ColliderType::AABB) {
      SphereCollider* sphere = static_cast<SphereCollider*>(a);
      AABBCollider* aabb = static_cast<AABBCollider*>(b);

      Vector3 aabbMin = aabb->GetMin();
      Vector3 aabbMax = aabb->GetMax();
      float radius = sphere->GetRadius();

      // 拡張AABBを作成（球の半径分拡大）
      Vector3 expandedMin = aabbMin - Vector3(radius, radius, radius);
      Vector3 expandedMax = aabbMax + Vector3(radius, radius, radius);

      // 球の中心が拡張AABBと交差する時刻を計算
      float tEntry[3], tExit[3];

      for (int i = 0; i < 3; ++i) {
         float v = 0.0f;
         float pos = 0.0f;

         if (i == 0) { v = velocity.x; pos = prevPos.x; }
         else if (i == 1) { v = velocity.y; pos = prevPos.y; }
         else { v = velocity.z; pos = prevPos.z; }

         float min = (i == 0) ? expandedMin.x : (i == 1) ? expandedMin.y : expandedMin.z;
         float max = (i == 0) ? expandedMax.x : (i == 1) ? expandedMax.y : expandedMax.z;

         if (std::abs(v) < 1e-6f) {
            if (pos < min || pos > max) {
               return { false, 0.0f };
            }
            tEntry[i] = -std::numeric_limits<float>::infinity();
            tExit[i] = std::numeric_limits<float>::infinity();
         } else {
            tEntry[i] = (min - pos) / v;
            tExit[i] = (max - pos) / v;

            if (tEntry[i] > tExit[i]) {
               std::swap(tEntry[i], tExit[i]);
            }
         }
      }

      float entryTime = std::max({ tEntry[0], tEntry[1], tEntry[2] });
      float exitTime = std::min({ tExit[0], tExit[1], tExit[2] });

      if (entryTime > exitTime || entryTime > 1.0f || exitTime < 0.0f) {
         return { false, 0.0f };
      }

      return { true, std::max(0.0f, entryTime) };
   }

   return { false, 0.0f };
}

void CollisionManager::CheckAllCollisions() {
   std::unordered_set<std::pair<Collider*, Collider*>, ColliderPairHash> currentCollisions;

   // すべてのコライダーペアをチェック
   for (size_t i = 0; i < colliders_.size(); ++i) {
      for (size_t j = i + 1; j < colliders_.size(); ++j) {
         Collider* a = colliders_[i];
         Collider* b = colliders_[j];

         // コライダーが無効化されている場合はスキップ
         if (!a->IsEnabled() || !b->IsEnabled()) continue;

         // コリジョンマトリクスで判定が無効なら処理しない
         if (!config_->IsCollisionEnabled(a->GetLayer(), b->GetLayer())) continue;

         auto pair = MakePair(a, b);
         bool isColliding = false;

         // 高速移動チェック - どちらかが高速移動していれば連続衝突検出を実行
         bool aFastMoving = IsFastMoving(a);
         bool bFastMoving = IsFastMoving(b);

         if (tunnelingPreventionEnabled_ && (aFastMoving || bFastMoving)) {
            std::pair<bool, float> sweptResult = { false, 0.0f };

            // コライダーのタイプに応じて適切な連続衝突検出を選択
            if (a->GetType() == ColliderType::AABB && b->GetType() == ColliderType::AABB) {
               sweptResult = SweptAABBCheck(a, b);
            } else if (a->GetType() == ColliderType::Sphere || b->GetType() == ColliderType::Sphere) {
               // どちらかが球体の場合、高速移動している方を第一引数にする
               if (aFastMoving) {
                  sweptResult = SweptSphereCheck(a, b);
               } else if (bFastMoving) {
                  sweptResult = SweptSphereCheck(b, a);
               }
            }

            if (sweptResult.first) {
               isColliding = true;
            }
         }

         // 通常の衝突判定（連続衝突検出で衝突していない、または低速移動の場合）
         if (!isColliding) {
            isColliding = a->CheckCollision(b);
         }

         if (isColliding) {
            currentCollisions.insert(pair);

            // 前フレームで衝突していなかった場合、Enter
            if (previousCollisions_.find(pair) == previousCollisions_.end()) {
               a->OnCollisionEnter(b);
               b->OnCollisionEnter(a);
            } else {
               // 前フレームも衝突していた場合、Stay
               a->OnCollisionStay(b);
               b->OnCollisionStay(a);
            }
         } else {
            // 前フレームで衝突していたが今フレームは離れた場合、Exit
            if (previousCollisions_.find(pair) != previousCollisions_.end()) {
               a->OnCollisionExit(b);
               b->OnCollisionExit(a);
            }
         }
      }
   }

   previousCollisions_ = std::move(currentCollisions);

   // 現在のフレームの位置を記録（次フレームでのトンネリング検出用）
   for (Collider* collider : colliders_) {
      if (collider && collider->IsEnabled()) {
         previousPositions_[collider] = collider->GetPosition();
      }
   }
}

void CollisionManager::Clear() {
   colliders_.clear();
   previousCollisions_.clear();
   previousPositions_.clear();
}
}
