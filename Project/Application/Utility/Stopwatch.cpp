#include "Stopwatch.h"

void Stopwatch::Start() {
    running_ = true;
    startTime_ = std::chrono::high_resolution_clock::now();
}

void Stopwatch::Stop() {
    endTime_ = std::chrono::high_resolution_clock::now();
    running_ = false;
}

void Stopwatch::Pause() {
    if (running_) {
        pauseTime_ = std::chrono::high_resolution_clock::now();
        running_ = false;
    }
}

void Stopwatch::Resume() {
    if (!running_) {
        auto now = std::chrono::high_resolution_clock::now();
        startTime_ += (now - pauseTime_);
        running_ = true;
    }
}

double Stopwatch::ElapsedMilliseconds() const {
    std::chrono::time_point<std::chrono::high_resolution_clock> endTime;
    if (running_) {
        endTime = std::chrono::high_resolution_clock::now();
    } else {
        endTime = endTime_;
    }
    return std::chrono::duration<double, std::milli>(endTime - startTime_).count();
}
