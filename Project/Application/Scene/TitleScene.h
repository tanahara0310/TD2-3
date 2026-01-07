#pragma once
#include "Scene/BaseScene.h"
#include "Application/Utility/Command/SceneCommandExecutor.h"

class TitleScene : public BaseScene {
public:
    /// @brief 初期化
    void Initialize(EngineSystem* engine) override;
    /// @brief 更新
    void Update() override;
    /// @brief 描画処理
    void Draw() override;
    /// @brief 解放
    void Finalize() override;

private:
    SceneCommandExecutor frameStartCommandExecutor_;
};