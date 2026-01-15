#include "RhythmTrigger.h"
#include "Application/SceneObjects/BeatController/BeatScheduler.h"
#include "Application/Utility/KeyBindConfig.h"

namespace {
    /// @brief リズムアクションに使うアクション名
    std::string TAG = "Start";
    /// @brief ノーツに対して成功とみなす範囲
    float SUCCESS_RANGE = 0.05f;
}

RhythmTrigger::RhythmTrigger(const BeatScheduler& beatScheduler) :
    beatScheduler_(beatScheduler),
    isSuccess_(false){}

void RhythmTrigger::Initialize() {
    isSuccess_ = false;
}

void RhythmTrigger::Update() {
    KeyBindConfig& keyBindConfig = KeyBindConfig::Instance();
    if (keyBindConfig.IsTrigger(TAG)) {
        float measurePos = beatScheduler_.GetCurrentMeasurePosition();
        // ノーツの位置に近ければ成功とする（例: ±0.1の範囲）
        const Measure& currentMeasure = beatScheduler_.GetCurrentMeasure();
        int beatsPerMeasure = currentMeasure.GetBeatsPerMeasure();
        isSuccess_ = false;
        for (int i = 0; i < beatsPerMeasure; ++i) {
            float beatPos = currentMeasure.GetBeatPosition(i);
            if (std::abs(measurePos - beatPos) <= SUCCESS_RANGE) {
                isSuccess_ = true;
                break;
            } else {
                isMissed_ = true;
            }
        }
    } else {
        isSuccess_ = false;
        isMissed_ = false;
    }
}

const bool RhythmTrigger::IsTriggered() const {
    KeyBindConfig& keyBindConfig = KeyBindConfig::Instance();
    return keyBindConfig.IsTrigger(TAG);
}

const bool RhythmTrigger::IsSuccess() const {
    return isSuccess_;
}

const bool RhythmTrigger::IsMissed() const {
    return isMissed_;
}

const bool RhythmTrigger::IsOnBeat() const {
    float measurePos = beatScheduler_.GetCurrentMeasurePosition();
    const Measure& currentMeasure = beatScheduler_.GetCurrentMeasure();
    int beatsPerMeasure = currentMeasure.GetBeatsPerMeasure();
    for (int i = 0; i < beatsPerMeasure; ++i) {
        float beatPos = currentMeasure.GetBeatPosition(i);
        if (std::abs(measurePos - beatPos) <= SUCCESS_RANGE) {
            return true;
        }
    }
    return false;
}
