#include "AABBCollider.h"
#include "SphereCollider.h"
#include "Engine/Utility/Collision/CollisionUtils.h"

#ifdef _DEBUG
#include "Engine/Graphics/Line/DebugLineDrawer.h"
#include "Engine/Graphics/Render/Line/LineRendererPipeline.h"
#include "Engine/Camera/ICamera.h"
#endif

AABBCollider::AABBCollider(GameObject* owner, const Vector3& size) {
   type_ = ColliderType::AABB;
   owner_ = owner;
   size_ = size;
}

bool AABBCollider::CheckCollision(Collider* other) const {
   if (!other) return false;

   if (other->GetType() == ColliderType::Sphere) {
      const SphereCollider& s = static_cast<const SphereCollider&>(*other);
      BoundingBox aabb = { GetMin(), GetMax() };
      CollisionUtils::Sphere sphere = { s.GetPosition(), s.GetRadius() };
      return CollisionUtils::IsColliding(sphere, aabb);
   } else if (other->GetType() == ColliderType::AABB) {
      const AABBCollider& a = static_cast<const AABBCollider&>(*other);
      BoundingBox aabb1 = { GetMin(), GetMax() };
      BoundingBox aabb2 = { a.GetMin(), a.GetMax() };
      return CollisionUtils::IsColliding(aabb1, aabb2);
   }
   return false;
}

void AABBCollider::SetSize(const Vector3& size) {
   size_ = size;
}

Vector3 AABBCollider::GetMin() const {
   return GetPosition() - size_ * 0.5f;
}

Vector3 AABBCollider::GetMax() const {
   return GetPosition() + size_ * 0.5f;
}

#ifdef _DEBUG
void AABBCollider::DrawDebug(LineRendererPipeline* pipeline, const ICamera* camera, const Vector3& color) const {
   if (!pipeline || !camera) return;

   Vector3 center = GetPosition();
   DebugLineDrawer::DrawBox(pipeline, camera, center, size_, color, 1.0f);
}
#endif
