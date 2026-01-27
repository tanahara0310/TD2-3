#pragma once
#include "CollisionLayer.h"
#include "MathCore.h"
#include <string>
#include <unordered_set>


namespace CoreEngine
{
    enum class ColliderType {
        None,
        Sphere,
        AABB,
    };

    class GameObject;

    class Collider {
    public:
        virtual ~Collider() = default;

        virtual bool CheckCollision(Collider* other) const = 0;

        Vector3 GetPosition() const;
        ColliderType GetType() const;

        void OnCollisionEnter(Collider* other);
        void OnCollisionStay(Collider* other);
        void OnCollisionExit(Collider* other);

        void SetLayer(CollisionLayer layer) { layer_ = layer; }
        CollisionLayer GetLayer() const { return layer_; }

        void SetOwner(GameObject* owner) { owner_ = owner; }
        GameObject* GetOwner() const { return owner_; }

        void SetEnabled(bool enabled) { isEnabled_ = enabled; }
        bool IsEnabled() const { return isEnabled_; }

        virtual void SetSize(const Vector3& size) { (void)size; }

        virtual void SetRadius(float radius) { (void)radius; }

        void AddTag(const std::string& tag) { tags_.insert(tag); }
        void RemoveTag(const std::string& tag) { tags_.erase(tag); }
        bool HasTag(const std::string& tag) const { return tags_.find(tag) != tags_.end(); }
        const std::unordered_set<std::string>& GetTags() const { return tags_; }
        void ClearTags() { tags_.clear(); }

        bool HasCommonTag(const Collider* other) const;

    protected:
        ColliderType type_ = ColliderType::None;
        GameObject* owner_ = nullptr;
        CollisionLayer layer_ = CollisionLayer::Default;
        bool isEnabled_ = true;
        std::unordered_set<std::string> tags_;
    };
}
