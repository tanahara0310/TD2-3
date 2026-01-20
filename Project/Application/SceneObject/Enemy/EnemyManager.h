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