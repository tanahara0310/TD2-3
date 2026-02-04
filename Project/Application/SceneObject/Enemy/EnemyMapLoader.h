#pragma once
#include <string>
#include <vector>
#include <externals/nlohmann/single_include/nlohmann/json.hpp>
class EnemyContainer;
class Player;
class BulletObjectContainer;

class EnemyMapLoader final{
public:
    EnemyMapLoader() = delete;
    explicit EnemyMapLoader(EnemyContainer* enemyManager,Player* player, BulletObjectContainer* spawnEffect);
    ~EnemyMapLoader() = default;

    // 敵配置データの保存と読み込み
    void SaveEnemyMap(const std::string& fileName);
    void LoadEnemyMap(const std::string& fileName);

    // 敵を再配置する
    void RespawnEnemies();

    // スタックから敵を出現させる
    void SpawnEnemiesFromStack(int index);
    // スタックからエフェクトを出現させる
    void SpawnEffectFromStack(int index);

    // スタックの数を取得する
    size_t GetEnemyMapStackSize() const;

private:
    Player* player_;
    EnemyContainer* enemyManager_;
    BulletObjectContainer* spawnEffect_;
    nlohmann::json enemyMapData_;
    std::vector<nlohmann::json> enemyMapStack_;
};