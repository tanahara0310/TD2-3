#pragma once
class BeatScheduler;

class RhythmTrigger final{
public:
    RhythmTrigger() = delete;
    explicit RhythmTrigger(const BeatScheduler& beatScheduler);
    ~RhythmTrigger() = default;
    /// @brief 初期化
    void Initialize();
    /// @brief 更新
    void Update();

    /// @brief そもそもボタンを押したかどうかを取得
    const bool IsTriggered() const;
    /// @brief 押した瞬間がノーツの上かどうかを取得(そのフレーム限定)
    const bool IsSuccess() const;
    /// @brief 押し損ねたかどうかを取得(そのフレーム限定)
    const bool IsMissed() const;
    
    /// @brief 現在の小節のビート上にいるかどうかを取得
    const bool IsOnBeat() const;

private:
    const BeatScheduler& beatScheduler_;
    bool isSuccess_;
    bool isMissed_;
};