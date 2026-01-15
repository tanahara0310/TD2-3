#pragma once
#include "Engine/ObjectCommon/GameObject.h"
class BeatScheduler;
#include "Engine/ObjectCommon/SpriteObject.h"

/// @brief BeatSchedulerの情報を表示するデバッグUIクラス
class BeatUi final {
public:
    /// @brief コンストラクタ・デストラクタ
    BeatUi() = delete;
    explicit BeatUi(SpriteObject& beatUiSprite, const BeatScheduler& beatScheduler);
    ~BeatUi() = default;
    
    /// @brief 初期化
    void Initialize();
    /// @brief 更新
    void Update();
    /// @brief 描画処理
    void Draw();

private:
    SpriteObject& beatUiSprite_;
    const BeatScheduler& beatScheduler_;
};