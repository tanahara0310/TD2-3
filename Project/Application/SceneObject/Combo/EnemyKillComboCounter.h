#pragma once
#include <externals/nlohmann/single_include/nlohmann/json.hpp>
class EnemyContainer;

class EnemyKillComboCounter {
public:
    EnemyKillComboCounter() = delete;
    explicit EnemyKillComboCounter(EnemyContainer* container);

    void Initialize();
    void Update();

    // コンボリセットタイマーをリセット
    void ResetTimer();

    // コンボ数をリセット
    void ResetCombo();
    // コンボ数を増加
    void IncrementCombo();
    // 現在のコンボ数を取得
    int GetCurrentCombo() const;

    // コンボタイマーを取得
    float GetComboTimer() const { return comboTimer_; }

private:
    float comboTimer_;
    int currentCombo_;
    nlohmann::json comboConfig_;

    EnemyContainer* container_;

    int oldDeathCount_;
};