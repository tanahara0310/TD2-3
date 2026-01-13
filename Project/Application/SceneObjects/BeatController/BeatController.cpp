#include "BeatController.h"

BeatController::BeatController() {
    beatInterval_ = 1.5f;
    timeSinceLastBeat_ = 0.0f;
}

void BeatController::Initialize() {
}

void BeatController::Update() {
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    lastTime_ = now;
    float deltaTime = std::chrono::duration<float>(now - lastTime_).count();
    timeSinceLastBeat_ += deltaTime;
    if (timeSinceLastBeat_ >= beatInterval_) {
        TriggerBeat();
        timeSinceLastBeat_ = 0.0f;
    }
}

void BeatController::RegisterBeatAction(const std::function<void()>& action) {
    beatActions_.push_back(action);
}

void BeatController::TriggerBeat() {
    for (const auto& action : beatActions_) {
        action();
    }
}
