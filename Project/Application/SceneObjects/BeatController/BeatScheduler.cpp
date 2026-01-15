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

    float elapsedTime = std::chrono::duration<float>(std::chrono::steady_clock::now() - measureStartTime_).count();

    if (elapsedTime >= measureDuration_) {
        measureStartTime_ = std::chrono::steady_clock::now();

        int beatsPerMeasure;
        float nextDuration;

        if (measureQueue_.empty()) {
            // 現在の小節の拍数を取得
            beatsPerMeasure = measures_.GetBufferPointer()[currentMeasureIndex_].GetBeatsPerMeasure();
            // デフォルトの小節長を計算（BPM維持したい場合はここで計算式を入れる）
            nextDuration = measureDuration_;
        } else {
            auto [queuedBeats, queuedDuration] = measureQueue_.front();
            measureQueue_.pop();
            beatsPerMeasure = queuedBeats;
            nextDuration = queuedDuration;
        }

        Measure newMeasure(beatsPerMeasure);
        measures_.Push(newMeasure);
        measureDuration_ = nextDuration; // 必ず新しい小節の長さを設定

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

bool BeatScheduler::IsPlaying() const {
    return isPlaying_;
}

float BeatScheduler::GetCurrentMeasureDuration() const {
    return measureDuration_;
}

float BeatScheduler::GetCurrentBPM() const {
    // BPM = 60 / (小節の長さ / 小節あたりの拍数)
    if (measureDuration_ <= 0.0f || GetCurrentMeasure().GetBeatsPerMeasure() <= 0.0f) {
        return 0.0f;
    }
    return 60.0f / (measureDuration_ / GetCurrentMeasure().GetBeatsPerMeasure());
}

rsize_t BeatScheduler::GetCurrentMeasureIndex() const {
    return currentMeasureIndex_;
}

size_t BeatScheduler::GetMeasureQueueSize() const {
    return measureQueue_.size();
}

const std::queue<std::pair<int, float>>& BeatScheduler::GetMeasureQueue() const {
    return measureQueue_;
}

void BeatScheduler::EnqueueMeasure(int beatsPerMeasure, float duration) {
    measureQueue_.emplace(beatsPerMeasure, duration);
}
