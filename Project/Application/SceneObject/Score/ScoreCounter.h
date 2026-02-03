#pragma once
#include <vector>

class ScoreCounter {
public:
    // シングルトン
    static ScoreCounter& GetInstance() {
        static ScoreCounter instance;
        return instance;
    }

    void Initialize();
    void AddScore(int points);
    int GetScore() const;
    void ResetScore();

    void AddHistoryScore();
    void SortScoreArrayDescending();
    std::vector<int> GetScoreArray() const { return scoreArray_; }

private:
    ScoreCounter() : score_(0) {}

    // 禁止コピーコンストラクタと代入演算子
    ScoreCounter(const ScoreCounter&) = default;
    ScoreCounter& operator=(const ScoreCounter&) = default;

    int score_;
    std::vector<int> scoreArray_;
};