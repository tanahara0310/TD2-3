#include "DummyEnemy.h"


DummyEnemy::DummyEnemy() :
    IEnemy("ApplicationAssets/Model/NormalEnemy.obj", "Texture/NormalEnemy.png") {

    CoreEngine::SoundManager* soundManager = GetEngineSystem()->GetComponent<CoreEngine::SoundManager>();
    if (!soundManager) {
        assert(false && "SoundManager not found");
    }
    soundResources_.clear();
    soundResources_["Die"] = soundManager->CreateSoundResource("Assets/ApplicationAssets/Sound/SE_EnemyDeath.mp3");

    collider_->SetRadius(1.5f);
    transform_.scale = { 1.0f,1.0f,1.0f };
    damageIntervalCounter_ = 0;
    maxDamageIntervalCounter_ = 15;
}

void DummyEnemy::Initialize() {
    hp_ = 5;
    SetActive(true);
    collider_->SetEnabled(true);
    collider_->SetRadius(2.0f);
    isActive_ = true;
    transform_.scale = { 1.0f,1.0f,1.0f };
}

void DummyEnemy::EnemyUpdate() {
    if (isAlive_) {
        transform_.rotate.y += 0.02f;
        if (damageIntervalCounter_ > 0) {
            transform_.scale.x = 1.0f + sinf(static_cast<float>(damageIntervalCounter_));
            transform_.scale.y = 1.0f + sinf(static_cast<float>(damageIntervalCounter_));
            transform_.scale.z = 1.0f + sinf(static_cast<float>(damageIntervalCounter_));
        } else {
            transform_.scale = { 1.0f,1.0f,1.0f };
        }
        

    } else {
        transform_.scale.x = 0.5f + sinf(transform_.scale.x) * 0.3f;
        transform_.scale.y = 0.5f + sinf(transform_.scale.y) * 0.3f;
        transform_.scale.z = 0.5f + sinf(transform_.scale.z) * 0.3f;
    }

    // ダメージ無敵時間のカウントダウン
    if (damageIntervalCounter_ > 0) {
        damageIntervalCounter_--;
    }
}

void DummyEnemy::PlaySE(const std::string& soundKey) {
    auto it = soundResources_.find(soundKey);
    if (it != soundResources_.end()) {
        soundResources_[soundKey]->Play(false);
    }
}
