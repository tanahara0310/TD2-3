#include "EnemyKillComboCounter.h"
#include "Application/SceneObject/Enemy/EnemyContainer.h"

EnemyKillComboCounter::EnemyKillComboCounter(EnemyContainer* container) :
    container_(container),
    currentCombo_(0) {
    comboConfig_["comboResetTime"] = 3.0f;
    oldDeathCount_ = 0;
}

void EnemyKillComboCounter::Initialize() {
    currentCombo_ = 0;
    oldDeathCount_ = 0;
}

void EnemyKillComboCounter::Update() {
    // コンボリセットタイマーの更新
    if (currentCombo_ > 0) {
        comboTimer_ -= 1.0f / 60.0f; // 仮に60FPSとして計算
        if (comboTimer_ <= 0.0f) {
            ResetCombo();
        }
    }

    // 敵の死亡数の変化をチェック
    if (container_) {
        auto deathList = container_->DeathEnemyList();
        int deathCount = static_cast<int>(deathList.size());
        if (deathCount > oldDeathCount_) {
            // 敵が新たに倒された場合、コンボを増加
            IncrementCombo();
            oldDeathCount_ = deathCount;
        } else if (deathCount < oldDeathCount_) {
            // 敵がリスポーンなどで減った場合、古いカウントを更新
            oldDeathCount_ = deathCount;
        }
    }
}

void EnemyKillComboCounter::ResetTimer() {
    comboTimer_ = comboConfig_["comboResetTime"].get<float>();
}

void EnemyKillComboCounter::ResetCombo() {
    currentCombo_ = 0;
    comboTimer_ = 0.0f;
}

void EnemyKillComboCounter::IncrementCombo() {
    currentCombo_++;
    comboTimer_ = comboConfig_["comboResetTime"].get<float>();
}

int EnemyKillComboCounter::GetCurrentCombo() const {
    return currentCombo_;
}
