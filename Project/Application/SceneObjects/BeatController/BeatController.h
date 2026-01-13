#pragma once
#include <vector>
#include <functional>
#include <chrono>

class BeatController {
public:
    BeatController();
    ~BeatController() = default;
    void Initialize();
    void Update();
    // ビートに合わせて何かをする関数を登録
    void RegisterBeatAction(const std::function<void()>& action);
private:
    std::vector<std::function<void()>> beatActions_;
    float beatInterval_; // ビートの間隔（秒）
    std::chrono::steady_clock::time_point lastTime_;
    float timeSinceLastBeat_;
    void TriggerBeat();
};