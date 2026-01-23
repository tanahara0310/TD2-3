#pragma once
#include <functional>

// 一定時間ごとに敵が出現するルールクラス
class TimeAndEnemyCountSpawnRule final {
public:
    TimeAndEnemyCountSpawnRule() = delete;
    explicit TimeAndEnemyCountSpawnRule(std::function<void()> spawnFunction, std::function<size_t()> enemyCountFunction);
    ~TimeAndEnemyCountSpawnRule() = default;
    // 更新処理
    void Update();
private:
    float spawnInterval_;      // 敵出現間隔
    float timeSinceLastSpawn_; // 最後の出現からの経過時間
    size_t maxEnemyCount_;    // 最大敵数

    std::function<void()> spawnFunction_; // 敵出現関数
    std::function<size_t()> enemyCountFunction_; // 敵数取得関数
};