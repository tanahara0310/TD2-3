#pragma once
#include "Application/Utility/RingBuffer.h"
#include "Measure.h"
#include <chrono>
#include <queue>
#include <nlohmann/single_include/nlohmann/json.hpp>

/// @brief 小節を時間で交換するクラス
class BeatScheduler final {
public:
    /// @brief小節を3つ分保持するリングバッファで管理
    BeatScheduler();
    ~BeatScheduler() = default;

    /// @brief 指定した拍子で小節を初期化
    void Initialize(int beatsPerMeasure);
    /// @brief 毎フレーム更新
    void Update();

    /// @brief 小節の時間を一時停止
    void Pause();
    /// @brief 小節の時間を再開
    void Resume();
    /// @brief 次に追加する小節の拍数と時間をキューに追加
    void EnqueueMeasure(int beatsPerMeasure, float duration);

    /// @brief 現在の小節を取得
    const Measure& GetCurrentMeasure() const;
    /// @brief 次の小節を取得
    const Measure& GetNextMeasure() const;
    /// @brief 現在の小節の位置を0.0～1.0の範囲で取得
    float GetCurrentMeasurePosition() const;

private:
    // 設定情報
    nlohmann::json config_;

    // 再生するかどうかのフラグ
    bool isPlaying_;
    // 再生していない時間の計測用
    std::chrono::steady_clock::time_point pauseStartTime_;

    // 小節を保持するリングバッファ
    RingBuffer<Measure> measures_;
    // 追加する小節の拍数と時間のペアのキュー
    std::queue<std::pair<int, float>> measureQueue_;
    
    // 現在の小節インデックス
    rsize_t currentMeasureIndex_;
    // 一小節にかかる時間
    float measureDuration_;
    // 小節開始時間の計測用
    std::chrono::steady_clock::time_point measureStartTime_;
};