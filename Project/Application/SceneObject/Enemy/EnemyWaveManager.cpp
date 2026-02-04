#include "EnemyWaveManager.h"
#include "Application/SceneObject/Enemy/EnemyMapLoader.h"

EnemyWaveManager::EnemyWaveManager(EnemyMapLoader* enemyLoader) {
    enemyMapLoader_ = enemyLoader;
    currentWaveNumber_ = 0;
}

void EnemyWaveManager::StartNextWave() {
    currentWaveNumber_++;
    // 敵を出現させる
    enemyMapLoader_->SpawnEnemiesFromStack(currentWaveNumber_ % enemyMapLoader_->GetEnemyMapStackSize());
    enemyMapLoader_->SpawnEffectFromStack((currentWaveNumber_ + 1) % enemyMapLoader_->GetEnemyMapStackSize());
}

int EnemyWaveManager::GetCurrentWaveNumber() const {
    return currentWaveNumber_;
}
