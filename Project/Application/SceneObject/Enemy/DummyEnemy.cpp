#include "DummyEnemy.h"

DummyEnemy::DummyEnemy() :
    IEnemy("ApplicationAssets/Model/white1x1Box.obj", "Texture/white1x1.png") {
}

void DummyEnemy::Initialize() {
    hp_ = 1;
    SetActive(true);
    collider_->SetEnabled(true);
}

void DummyEnemy::EnemyUpdate() {
    transform_.rotate.y += 0.02f;
}

void DummyEnemy::OnCollisionEnter(CoreEngine::GameObject* other) {
    (void)other;
    SetActive(false);
}
