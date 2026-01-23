#pragma once
#include <chrono>
class Stopwatch {
public:
    void Start();
    void Stop();
    void Pause();
    void Resume();
    double ElapsedMilliseconds() const;

    bool IsRunning() const { return running_; }

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime_;
    std::chrono::time_point<std::chrono::high_resolution_clock> endTime_;
    std::chrono::time_point<std::chrono::high_resolution_clock> pauseTime_;
    bool running_;
};