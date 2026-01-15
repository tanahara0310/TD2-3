#include "BeatScheduler.h"


BeatScheduler::BeatScheduler() : 
    measures_(3),
    currentMeasureIndex_(0),
    measureDuration_(2.0f),
    measureStartTime_(std::chrono::steady_clock::now()),
    isPlaying_(true)
{}

void BeatScheduler::Initialize(int beatsPerMeasure) {
    measures_.Clear();
    for (size_t i = 0; i < measures_.Capacity(); ++i) {
        Measure measure(beatsPerMeasure);
        measures_.Push(measure);
    }

    // 現在の小節を取得
    currentMeasureIndex_ = measures_.GetHeadIndex();

    isPlaying_ = true;
}

void BeatScheduler::Update() {
    if (!isPlaying_) {
        return;
    }

    // 小節の経過時間を計算
    float elapsedTime = std::chrono::duration<float>(std::chrono::steady_clock::now() - measureStartTime_).count();
    
    // 小節の時間を超えたら次の小節へ移動
    if (elapsedTime >= measureDuration_) {
        // 小節のスタート時間を更新
        measureStartTime_ = std::chrono::steady_clock::now();
        // 次の小節へ移動
        if (measureQueue_.empty()) {
            // キューが空なら同じ拍数で新しい小節を追加
            Measure newMeasure(measures_.GetBufferPointer()[currentMeasureIndex_].GetBeatsPerMeasure());
            measures_.Push(newMeasure);
        } else {
            // キューから拍数と時間を取得して新しい小節を追加
            auto [beatsPerMeasure, duration] = measureQueue_.front();
            measureQueue_.pop();
            Measure newMeasure(beatsPerMeasure);
            measures_.Push(newMeasure);
            measureDuration_ = duration;
        }
        // 現在の小節インデックスを更新
        currentMeasureIndex_ = measures_.GetHeadIndex();
    }
}

void BeatScheduler::Pause() {
    if (!isPlaying_) {
        return;
    }
    isPlaying_ = false;
    pauseStartTime_ = std::chrono::steady_clock::now();
}

void BeatScheduler::Resume() {
    if (isPlaying_) {
        return;
    }
    isPlaying_ = true;
    // ポーズしていた時間を計算
    auto pauseDuration = std::chrono::steady_clock::now() - pauseStartTime_;
    // 小節のスタート時間を調整
    measureStartTime_ += pauseDuration;
}

const Measure& BeatScheduler::GetCurrentMeasure() const {
    return measures_.Get(currentMeasureIndex_);
}

const Measure& BeatScheduler::GetNextMeasure() const {
    return measures_.Get((currentMeasureIndex_ + 1) % measures_.Capacity());
}

float BeatScheduler::GetCurrentMeasurePosition() const {
    if (isPlaying_) {
        float elapsedTime = std::chrono::duration<float>(std::chrono::steady_clock::now() - measureStartTime_).count();
        return elapsedTime / measureDuration_;
    } else {
        float elapsedTime = std::chrono::duration<float>(pauseStartTime_ - measureStartTime_).count();
        return elapsedTime / measureDuration_;
    }
}

void BeatScheduler::EnqueueMeasure(int beatsPerMeasure, float duration) {
    measureQueue_.emplace(beatsPerMeasure, duration);
}
