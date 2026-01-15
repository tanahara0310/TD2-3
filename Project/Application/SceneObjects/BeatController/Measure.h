#pragma once
#include <vector>

/// @brief 一小節に含まれる拍子の位置を0.0～1.0の範囲で管理するクラス
class Measure final{
public:
    /// @brief デフォルトコンストラクタ、4拍子で初期化
    Measure();
    /// @brief 一小節に含まれる拍数で初期化、等間隔に拍を配置する
    explicit Measure(int beatsPerMeasure);
    ~Measure() = default;

    /// 小節を任意の拍数で初期化
    void Initialize(int beatsPerMeasure);
    /// 現在の拍数で等間隔に拍を再配置
    void RecalculateBeatPositions();

    /// @brief 一小節あたりの拍数を取得
    int GetBeatsPerMeasure() const;
    /// @brief 指定した拍番号の位置を0~1の範囲で取得
    float GetBeatPosition(int beatIndex) const;
    /// @brief 範囲内に拍子があるかどうかを判定
    bool IsBeatInRange(float start, float end) const;

private:
    int beatsPerMeasure_; // 一小節あたりの拍数
    std::vector<float> beatPositions_; // 各拍の位置を格納
};