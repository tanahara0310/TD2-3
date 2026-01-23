#pragma once
#include <functional>
#include <vector>

// ゲームオーバーやゲームクリアの判定を管理するクラス
class GameResultManager final {
public:
    void Initialize();
    void Update();

    // ゲームオーバー条件の追加
    void AddGameOverCondition(const std::function<bool()>& condition);
    // ゲームクリア条件の追加
    void AddGameClearCondition(const std::function<bool()>& condition);

    // ゲームオーバー・ゲームクリアの判定
    bool CheckGameOver() const;
    bool CheckGameClear() const;

private:
    std::vector<std::function<bool()>> gameOverConditions_;
    std::vector<std::function<bool()>> gameClearConditions_;
};
