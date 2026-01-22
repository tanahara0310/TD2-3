#pragma once
#include <vector>
#include <unordered_map>
#include "IEnemy.h"
#include "Scene/BaseScene.h"

class EnemyManager final {
public:

    EnemyManager()= delete;
    explicit EnemyManager(CoreEngine::BaseScene* scene);
    ~EnemyManager() = default;
    void Update();

    template<typename T>
    void SpawnEnemy(const CoreEngine::Vector3& position) {
        // 再利用可能な敵がいるか確認
        if (enemyMap_.find(typeid(T).name()) != enemyMap_.end()) {
            for (auto* enemy : enemyMap_[typeid(T).name()]) {
                if (!enemy->IsActive()) {
                    enemy->Initialize();
                    enemy->SetActive(true);
                    enemy->GetTransform() = position;
                    return;
                }
            }
        }
        // 新規作成
        enemyMap_[typeid(T).name()].push_back(scene_->CreateObject<T>());
        auto& enemyList = enemyMap_[typeid(T).name()];
        auto* newEnemy = enemyList.back();
        newEnemy->GetTransform() = position;
    }

    std::unordered_map<std::string, std::vector<IEnemy*>>& GetEnemyMap() {
        return enemyMap_;
    }

private:
    CoreEngine::BaseScene* scene_ = nullptr;
    std::unordered_map<std::string, std::vector<IEnemy*>> enemyMap_;
};