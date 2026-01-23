#include "TimeAndEnemyCountSpawnRule.h"

TimeAndEnemyCountSpawnRule::TimeAndEnemyCountSpawnRule(
    std::function<void()> spawnFunction, std::function<size_t()> enemyCountFunction) {
    spawnInterval_ = 5.0f; // 5秒ごとに出現
    timeSinceLastSpawn_ = 0.0f;
    maxEnemyCount_ = 3; // 最大3体まで
    spawnFunction_ = spawnFunction;
    enemyCountFunction_ = enemyCountFunction;
}

void TimeAndEnemyCountSpawnRule::Update() {
    // 経過時間を更新
    timeSinceLastSpawn_ += 1.0f / 60.0f; // 仮に60FPSとして計算
    // 一定時間経過したら敵を出現させる
    if (timeSinceLastSpawn_ >= spawnInterval_) {
        // 敵の数が一定数以下の場合のみ出現
        if (enemyCountFunction_ && enemyCountFunction_() > maxEnemyCount_) { 
            return;
        }

        spawnFunction_();
        timeSinceLastSpawn_ = 0.0f;
    }
}
