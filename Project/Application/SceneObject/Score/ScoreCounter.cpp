#include "ScoreCounter.h"
#include <algorithm>

void ScoreCounter::Initialize() {
    score_ = 0;
    scoreArray_.clear();
}

void ScoreCounter::AddScore(int points) {
    score_ += points;
}

int ScoreCounter::GetScore() const {
    return score_;
}

void ScoreCounter::ResetScore() {
    score_ = 0;
}

void ScoreCounter::AddHistoryScore() {
    scoreArray_.push_back(score_);
}

void ScoreCounter::SortScoreArrayDescending() {
    std::sort(scoreArray_.begin(), scoreArray_.end(), std::greater<int>());
}
