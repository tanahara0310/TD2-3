#include "FollowEnemy.h"

FollowEnemy::FollowEnemy(CoreEngine::Vector3* targetPos) :
    IEnemy("ApplicationAssets/Model/white1x1Box.obj", "Texture/white1x1.png"),
    targetPos_(targetPos) {
    CoreEngine::SoundManager* soundManager = GetEngineSystem()->GetComponent<CoreEngine::SoundManager>();
    if (!soundManager) {
        assert(false && "SoundManager not found");
    }
    soundResources_.clear();
    soundResources_["Die"] = soundManager->CreateSoundResource("Assets/ApplicationAssets/Sound/SE_EnemyDeath.mp3");
    collider_->SetRadius(1.5f);
    transform_.scale = { 2.0f,2.0f,2.0f };
}

void FollowEnemy::Initialize() {
    hp_ = 1;
    SetActive(true);
    collider_->SetEnabled(true);
    collider_->SetRadius(1.5f);
    isActive_ = true;
    transform_.scale = { 2.0f,2.0f,2.0f };
}

void FollowEnemy::EnemyUpdate() {
    if (isAlive_) {
        CoreEngine::Vector3 direction = (*targetPos_) - transform_.translate;
        direction = CoreEngine::Math::Vector::Normalize(direction);
        
        transform_.translate += direction * (2.0f / 60.0f);
        CoreEngine::Vector3 toTarget = (*targetPos_) - transform_.translate;
        toTarget.y = 0.0f; // 水平方向のみ考慮
        toTarget = CoreEngine::Math::Vector::Normalize(toTarget);
        float targetYaw = atan2f(toTarget.x, toTarget.z);
        transform_.rotate.y = targetYaw;
    } else {
        transform_.scale.x = 0.5f + sinf(transform_.scale.x) * 0.3f;
        transform_.scale.y = 0.5f + sinf(transform_.scale.y) * 0.3f;
        transform_.scale.z = 0.5f + sinf(transform_.scale.z) * 0.3f;
    }
}

void FollowEnemy::OnCollisionEnter(CoreEngine::GameObject* other) {
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

void FollowEnemy::PlaySE(const std::string& soundKey) {
    auto it = soundResources_.find(soundKey);
    if (it != soundResources_.end()) {
        soundResources_[soundKey]->Play(false);
    }
}
