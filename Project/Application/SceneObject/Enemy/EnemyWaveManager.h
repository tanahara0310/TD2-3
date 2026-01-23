#pragma once
class EnemyMapLoader;

// 敵のウェーブ管理クラス
class EnemyWaveManager final {
public:
    EnemyWaveManager() = delete;
    explicit EnemyWaveManager(EnemyMapLoader* enemyLoader);
    ~EnemyWaveManager() = default;

    void StartNextWave();
    int GetCurrentWaveNumber() const;

private:
    int currentWaveNumber_;
    EnemyMapLoader* enemyMapLoader_;
};