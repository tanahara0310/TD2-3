#include "EnemyContainer.h"

EnemyContainer::EnemyContainer(CoreEngine::BaseScene* scene) :
    scene_(scene) {
}

void EnemyContainer::Update() {
    for (auto& [typeName, enemyList] : enemyMap_) {
        for (auto& enemy : enemyList) {
            if (enemy->IsActive()) {
                enemy->GetCollider()->SetEnabled(true);
                enemy->EnemyUpdate();
                enemy->Update();
            } else {
                enemy->GetCollider()->SetEnabled(false);
            }
        }
    }
}

std::vector<IEnemy*> EnemyContainer::DeathEnemyList() {
    std::vector<IEnemy*> result;
    for (auto& [typeName, enemyList] : enemyMap_) {
        for (auto& enemy : enemyList) {
            if (!enemy->IsAlive() && enemy->IsActive()) {
                result.push_back(enemy);
            }
        }
    }
    return result;
}

size_t EnemyContainer::GetAliveEnemyCount() const {
    size_t count = 0;
    for (const auto& [typeName, enemyList] : enemyMap_) {
        for (const auto& enemy : enemyList) {
            if (enemy->IsAlive() && enemy->IsActive()) {
                count++;
            }
        }
    }
    return count;
}
