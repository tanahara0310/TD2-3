#pragma once
#include "../PostEffectBase.h"
#include <wrl.h>
#include <d3d12.h>

namespace CoreEngine
{
class RetroGrid : public PostEffectBase {
public:
    /// @brief レトログリッドパラメータ構造体
    struct RetroGridParams {
        float time = 0.0f;              // アニメーション時間
        float gridSize = 20.0f;         // グリッドのサイズ (10.0-50.0)
        float lineWidth = 0.05f;        // ラインの太さ (0.01-0.2)
        float scanlineSpeed = 1.0f;     // スキャンラインの速度 (0.0-5.0)
        float scanlineIntensity = 0.3f; // スキャンラインの強度 (0.0-1.0)
        float gridColorR = 0.0f;        // グリッドカラーR (0.0-1.0)
        float gridColorG = 1.0f;        // グリッドカラーG (0.0-1.0)
        float gridColorB = 1.0f;        // グリッドカラーB (0.0-1.0)
        float gridAlpha = 0.5f;         // グリッドの透明度 (0.0-1.0)
        float horizonGlow = 0.8f;       // 地平線のグロー (0.0-2.0)
        float perspectiveStrength = 0.6f; // パースの強度 (0.0-1.0)
        float noiseIntensity = 0.1f;    // ノイズの強度 (0.0-1.0)
        float padding = 0.0f;           // パディング
    };

public:
    RetroGrid() = default;
    ~RetroGrid() = default;

    /// @brief 初期化
    void Initialize(class DirectXCommon* dxCommon);

    /// @brief 更新（時間を進める）
    void Update(float deltaTime);

    /// @brief ImGuiでパラメータを調整
    void DrawImGui() override;

    /// @brief パラメータを取得
    const RetroGridParams& GetParams() const { return params_; }

    /// @brief パラメータを設定
    void SetParams(const RetroGridParams& params);

protected:
    const std::wstring& GetPixelShaderPath() const override;
    void BindOptionalCBVs(ID3D12GraphicsCommandList* commandList) override;

private:
    /// @brief 定数バッファの作成
    void CreateConstantBuffer();
    
    /// @brief 定数バッファの更新
    void UpdateConstantBuffer();

private:
    RetroGridParams params_;
    Microsoft::WRL::ComPtr<ID3D12Resource> constantBuffer_;
    RetroGridParams* mappedData_ = nullptr;
};
}
