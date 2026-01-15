#pragma once
#include "Engine/ObjectCommon/GameObject.h"
#include "Engine/ObjectCommon/SpriteObject.h"
#include <vector>

class BeatScheduler;
class RhythmTrigger;

/// @brief BeatSchedulerの情報を表示するデバッグUIクラス
class BeatUi final {
public:
    /// @brief コンストラクタ・デストラクタ
    BeatUi() = delete;
    explicit BeatUi(
        SpriteObject& beatUiSprite,
        SpriteObject& barSprite,
        std::vector<SpriteObject*> noteSprites,
        const BeatScheduler& beatScheduler,
        const RhythmTrigger& rhythmTrigger);
    ~BeatUi() = default;
    
    /// @brief 初期化
    void Initialize();
    /// @brief 更新
    void Update();
    /// @brief 描画処理
    void Draw();

private:
    // 描画用スプライト
    SpriteObject& beatUiSprite_;
    SpriteObject& beatBarUiSprite_;
    std::vector<SpriteObject*> noteSprites_;

    // 参照用ビートスケジューラー
    const BeatScheduler& beatScheduler_;
    const RhythmTrigger& rhythmTrigger_;

    // メンバ変数
    float barWidth_;
    Vector3 defaultPos_;
    Vector4 barColor_;
    float shakeTimer_;
};