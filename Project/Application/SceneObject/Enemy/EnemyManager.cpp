#include "EnemyManager.h"

EnemyManager::EnemyManager(CoreEngine::BaseScene* scene) :
    scene_(scene) {
}

void EnemyManager::Update() {
    for (auto& [typeName, enemyList] : enemyMap_) {
        for (auto& enemy : enemyList) {
            if (enemy->IsActive()) {
                enemy->EnemyUpdate();
                enemy->Update();
            }
        }
    }
}
