#pragma once
#include <string>
#include <externals/nlohmann/single_include/nlohmann/json.hpp>
class EnemyContainer;

class EnemyMapLoader final{
public:
    EnemyMapLoader() = delete;
    explicit EnemyMapLoader(EnemyContainer* enemyManager);
    ~EnemyMapLoader() = default;

    // 敵配置データの保存と読み込み
    void SaveEnemyMap(const std::string& fileName);
    void LoadEnemyMap(const std::string& fileName);

    // 敵を再配置する
    void RespawnEnemies();

private:
    EnemyContainer* enemyManager_;
    nlohmann::json enemyMapData_;
};