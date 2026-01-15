#pragma once
#include "Application/Utility/RingBuffer.h"
#include "Measure.h"

/// @brief 正規化された拍子位置に基づいて
class BeatScheduler final {
public:
    BeatScheduler();
    ~BeatScheduler() = default;
    void Initialize(int beatsPerMeasure);
    void Update(float deltaTime);
private:
    RingBuffer<Measure> measures_;
};