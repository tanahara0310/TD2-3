#include "EnemyMapLoader.h"
#include "Application/SceneObject/Player/Player.h"
#include "Application/SceneObject/Enemy/EnemyContainer.h"
#include "Application/SceneObject/Enemy/AllEnemy.h"
#include <fstream>

namespace {
    const std::string kEnemyKey = "enemies";
    const std::string kEnemyMapFilePath = "Assets/ApplicationAssets/EnemySpawnMapData/";
}

EnemyMapLoader::EnemyMapLoader(EnemyContainer* enemyManager, Player* player) :
    enemyManager_(enemyManager),
    player_(player) {
    enemyMapStack_.clear();
}

void EnemyMapLoader::SaveEnemyMap(const std::string& fileName) {
    try
    {
        // 敵配置データの取得
        auto enemyMapData = enemyManager_->GetEnemyMap();
        for (const auto& [key, value] : enemyMapData)
        {
            for (const auto& enemy : value) {
                enemyMapData_[kEnemyKey].push_back(
                    { key,
                        { enemy->GetTransform().x,
                            enemy->GetTransform().y,
                            enemy->GetTransform().z } });
            }
        }
        // JSONファイルに保存
        std::ofstream ofs(kEnemyMapFilePath + fileName);
        ofs << enemyMapData_.dump(4);
    }
    catch (const std::exception&)
    {
    }
}

void EnemyMapLoader::LoadEnemyMap(const std::string& fileName) {
    try
    {
        std::ifstream ifs(kEnemyMapFilePath + fileName);
        ifs >> enemyMapData_;
        enemyMapStack_.push_back(enemyMapData_);
    }
    catch (const std::exception&)
    {

    }
}

void EnemyMapLoader::RespawnEnemies() {
    try
    {
        for (const auto& enemyData : enemyMapData_[kEnemyKey])
        {
            std::string enemyType = enemyData[0];
            CoreEngine::Vector3 position{
                enemyData[1][0].get<float>(),
                enemyData[1][1].get<float>(),
                enemyData[1][2].get<float>()
            };
            // 敵の再配置
            if (enemyType == typeid(DummyEnemy).name()) {
                enemyManager_->SpawnEnemy<DummyEnemy>(position);
            } else if (enemyType == typeid(FollowEnemy).name()) {
                // 仮にターゲット位置を原点に設定
                enemyManager_->SpawnEnemy<FollowEnemy>(position,player_->GetPosPtr());
            }
            // 他の敵タイプもここに追加
        }
    }
    catch (const std::exception&)
    {
    }
}

void EnemyMapLoader::SpawnEnemiesFromStack(int index) {
    try
    {
        if (index < 0 || index >= static_cast<int>(enemyMapStack_.size())) {
            return;
        }
        enemyMapData_ = enemyMapStack_[index];
        RespawnEnemies();
    }
    catch (const std::exception&)
    {
    }
}

size_t EnemyMapLoader::GetEnemyMapStackSize() const {
    return enemyMapStack_.size();
}
