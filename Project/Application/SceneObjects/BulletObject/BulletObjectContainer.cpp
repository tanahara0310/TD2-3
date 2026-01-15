#include "BulletObjectContainer.h"

BulletObjectContainer::BulletObjectContainer(uint32_t num) {
    bulletObjects_.resize(num);
    for (auto& obj : bulletObjects_) {
        obj = nullptr;
    }
}

void BulletObjectContainer::Initialize() {
    for (auto& obj : bulletObjects_) {
        if (obj) {
            obj->Initialize();
        }
    }
}

void BulletObjectContainer::Update() {
    for (auto& obj : bulletObjects_) {
        if (obj) {
            obj->Update();
        }
    }
}

void BulletObjectContainer::Draw(const ICamera* camera) {
    for (auto& obj : bulletObjects_) {
        if (obj) {
            obj->Draw(camera);
        }
    }
}

void BulletObjectContainer::SetActiveAll(bool isActive) {
    for (auto& obj : bulletObjects_) {
        if (obj) {
            obj->SetActive(isActive);
        }
    }
}

std::vector<BulletModel*>& BulletObjectContainer::GetBulletObjects() {
    return bulletObjects_;
}

int BulletObjectContainer::Spawn(const Vector3& pos, const Vector3& rotate, const Vector3& scale) {
    for (auto& obj : bulletObjects_) {
        if (obj && !obj->IsActive()) {
            obj->GetTransform().translate = pos;
            obj->GetTransform().rotate = rotate;
            obj->GetTransform().scale = scale;
            obj->GetTransform().TransferMatrix();
            obj->SetActive(true);
            // アクティブ化したオブジェクトのインデックスを返す
            return static_cast<int>(&obj - &bulletObjects_[0]);
        }
    }
    return -1; // 空きがなかった場合は-1を返す
}
