#pragma once
#include "../PostEffectBase.h"
#include <wrl.h>
#include <d3d12.h>

namespace CoreEngine
{
class TransitionEffect : public PostEffectBase {
public:
    /// @brief トランジションタイプ
    enum class TransitionType {
        Circle = 0,      // 円形トランジション
        Slide = 1,       // スライドトランジション
        Glitch = 2,      // グリッチトランジション
        Pixelate = 3,    // ピクセル分解トランジション
        Wipe = 4,        // ワイプトランジション
        Blind = 5,       // ブラインドトランジション
        ZoomBlur = 6,    // ズームブラートランジション
        Mosaic = 7       // モザイクトランジション
    };

    /// @brief トランジションパラメータ構造体
    struct TransitionParams {
        // float4境界 #1 (16bytes)
        float progress = 0.0f;              // トランジション進行度 (0.0-1.0)
        int transitionType = 0;             // トランジションタイプ
        float centerX = 0.5f;               // 中心X座標 (0.0-1.0)
        float centerY = 0.5f;               // 中心Y座標 (0.0-1.0)
        
        // float4境界 #2 (16bytes)
        float smoothness = 0.05f;           // エッジのぼかし具合
        float slideAngle = 0.0f;            // スライド角度（度）
        float glitchIntensity = 1.0f;       // グリッチ強度
        float pixelSize = 1.0f;             // ピクセルサイズ
        
        // float4境界 #3 (16bytes)
        float aspectRatio = 1.777f;         // アスペクト比補正 (16:9)
        float maskColorR = 0.0f;            // マスク色R
        float maskColorG = 0.0f;            // マスク色G
        float maskColorB = 0.0f;            // マスク色B
        
        // float4境界 #4 (16bytes)
        float blindCount = 10.0f;           // ブラインドの数
        float randomSeed = 0.0f;            // ランダムシード
        float waveFrequency = 5.0f;         // 波の周波数
        float waveAmplitude = 0.02f;        // 波の振幅
    };

public:
    TransitionEffect() = default;
    ~TransitionEffect() = default;

    /// @brief 初期化
    void Initialize(class DirectXCommon* dxCommon);

    /// @brief ImGuiでパラメータを調整
    void DrawImGui() override;

    /// @brief パラメータを取得
    const TransitionParams& GetParams() const { return params_; }

    /// @brief パラメータを設定
    void SetParams(const TransitionParams& params);

    /// @brief トランジション進行度を設定
    void SetProgress(float progress);

    /// @brief トランジションタイプを設定
    void SetTransitionType(TransitionType type);

    /// @brief 定数バッファを強制的に更新
    void ForceUpdateConstantBuffer();

protected:
    const std::wstring& GetPixelShaderPath() const override;
    void BindOptionalCBVs(ID3D12GraphicsCommandList* commandList) override;

private:
    /// @brief 定数バッファの作成
    void CreateConstantBuffer();

    /// @brief 定数バッファを更新
    void UpdateConstantBuffer();

private:
    TransitionParams params_;
    Microsoft::WRL::ComPtr<ID3D12Resource> constantBuffer_;
    class DirectXCommon* dxCommon_ = nullptr;
};

} // namespace CoreEngine
