#include "GameResultManager.h"

void GameResultManager::Initialize() {
}

void GameResultManager::Update() {
}

void GameResultManager::AddGameOverCondition(const std::function<bool()>& condition) {
    gameOverConditions_.push_back(condition);
}

void GameResultManager::AddGameClearCondition(const std::function<bool()>& condition) {
    gameClearConditions_.push_back(condition);
}

bool GameResultManager::CheckGameOver() const {
    for (const auto& condition : gameOverConditions_) {
        if (condition()) {
            return true;
        }
    }
    return false;
}

bool GameResultManager::CheckGameClear() const {
    for (const auto& condition : gameClearConditions_) {
        if (condition()) {
            return true;
        }
    }
    return false;
}
