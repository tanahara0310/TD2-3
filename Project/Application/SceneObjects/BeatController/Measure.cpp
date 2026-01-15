#include "Measure.h"

Measure::Measure(int beatsPerMeasure) :
    beatsPerMeasure_(beatsPerMeasure){
    RecalculateBeatPositions();
}

void Measure::Initialize(int beatsPerMeasure) {
    beatsPerMeasure_ = beatsPerMeasure;
    RecalculateBeatPositions();
}

void Measure::RecalculateBeatPositions() {
    beatPositions_.clear();
    for (int i = 0; i < beatsPerMeasure_; ++i) {
        float position = static_cast<float>(i) / static_cast<float>(beatsPerMeasure_);
        beatPositions_.push_back(position);
    }
}

int Measure::GetBeatsPerMeasure() const {
    return beatsPerMeasure_;
}

float Measure::GetBeatPosition(int beatIndex) const {
    if (beatIndex < 0 || beatIndex >= beatsPerMeasure_) {
        return -1.0f; // 無効なインデックスの場合は-1を返す
    }
    return beatPositions_[beatIndex];
}

bool Measure::IsBeatInRange(float start, float end) const {
    for (const auto& position : beatPositions_) {
        if (position >= start && position < end) {
            return true;
        }
    }
    return false;
}
