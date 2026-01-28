#pragma once
#include <vector>
#include <unordered_map>
#include "IEnemy.h"
#include "Scene/BaseScene.h"

class EnemyContainer final {
public:

    EnemyContainer()= delete;
    explicit EnemyContainer(CoreEngine::BaseScene* scene);
    ~EnemyContainer() = default;
    void Update();

    template<typename T, typename... Args>
    void SpawnEnemy(const CoreEngine::Vector3& position, Args&&... args) {
        // 再利用可能な敵がいるか確認
        if (enemyMap_.find(typeid(T).name()) != enemyMap_.end()) {
            for (auto* enemy : enemyMap_[typeid(T).name()]) {
                if (!enemy->IsActive()) {
                    enemy->Initialize();
                    enemy->SetActive(true);
                    enemy->GetTransform() = position;
                    enemy->SetAlive(true);
                    return;
                }
            }
        }
        // 新規作成（可変長引数を渡す）
        enemyMap_[typeid(T).name()].push_back(scene_->CreateObject<T>(std::forward<Args>(args)...));
        auto& enemyList = enemyMap_[typeid(T).name()];
        auto* newEnemy = enemyList.back();
        newEnemy->GetTransform() = position;
    }

    std::unordered_map<std::string, std::vector<IEnemy*>>& GetEnemyMap() {
        return enemyMap_;
    }

    // 死んでいる敵のリスト
    std::vector<IEnemy*> DeathEnemyList();
    size_t GetAliveEnemyCount() const;

private:
    CoreEngine::BaseScene* scene_ = nullptr;
    std::unordered_map<std::string, std::vector<IEnemy*>> enemyMap_;
};