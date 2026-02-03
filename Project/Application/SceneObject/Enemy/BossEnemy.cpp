#include "BossEnemy.h"

BossEnemy::BossEnemy() :
    IEnemy("ApplicationAssets/Model/white1x1Box.obj", "Texture/white1x1.png") {
    CoreEngine::SoundManager* soundManager = GetEngineSystem()->GetComponent<CoreEngine::SoundManager>();
    if (!soundManager) {
        assert(false && "SoundManager not found");
    }
    soundResources_.clear();
    soundResources_["Die"] = soundManager->CreateSoundResource("Assets/ApplicationAssets/Sound/SE_EnemyDeath.mp3");
    collider_->SetRadius(3.5f);
    transform_.scale = { 5.0f,5.0f,5.0f };
    collider_->SetLayer(CoreEngine::CollisionLayer::Enemy);
}

void BossEnemy::Initialize() {
    hp_ = 1000000;
    SetActive(true);
    collider_->SetEnabled(true);
    collider_->SetRadius(2.0f);
    isActive_ = true;
    transform_.scale = { 5.0f,5.0f,5.0f };
}

void BossEnemy::EnemyUpdate() {
    if (isAlive_) {
        transform_.rotate.y += 0.02f;
    } else {
        transform_.scale.x = 0.5f + sinf(transform_.scale.x) * 0.3f;
        transform_.scale.y = 0.5f + sinf(transform_.scale.y) * 0.3f;
        transform_.scale.z = 0.5f + sinf(transform_.scale.z) * 0.3f;
    }
}

void BossEnemy::OnCollisionEnter(CoreEngine::GameObject* other) {
    // 無効状態または非生存状態なら処理しない
    if (!isActive_ || !isAlive_) {
        return;
    }



    // ダメージ判定
    if (other->GetTag() == std::string("PlayerAttack")) {
        hp_--;
        if (hp_ <= 0) {
            isAlive_ = false;
            collider_->SetEnabled(false);
        }
    }
}

void BossEnemy::PlaySE(const std::string& soundKey) {
    auto it = soundResources_.find(soundKey);
    if (it != soundResources_.end()) {
        soundResources_[soundKey]->Play(false);
    }
}
