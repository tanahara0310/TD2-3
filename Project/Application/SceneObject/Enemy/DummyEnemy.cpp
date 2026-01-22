#include "DummyEnemy.h"

DummyEnemy::DummyEnemy() :
    IEnemy("ApplicationAssets/Model/white1x1Box.obj", "Texture/white1x1.png") {
}

void DummyEnemy::Initialize() {
    hp_ = 1;
    SetActive(true);
    collider_->SetEnabled(true);
    isActive_ = true;
}

void DummyEnemy::EnemyUpdate() {
    if (isAlive_) {
        transform_.rotate.y += 0.02f;
    } else {
        transform_.scale.x = 0.5f + sinf(transform_.scale.x) * 0.3f;
        transform_.scale.y = 0.5f + sinf(transform_.scale.y) * 0.3f;
        transform_.scale.z = 0.5f + sinf(transform_.scale.z) * 0.3f;
    }
}

void DummyEnemy::OnCollisionEnter(CoreEngine::GameObject* other) {
    if (other->GetObjectName() == std::string("Ball")) {
        hp_--;
        if (hp_ <= 0) {
            isAlive_ = false;
            collider_->SetEnabled(false);
        }
    }
}