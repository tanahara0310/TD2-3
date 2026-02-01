#pragma once
#include "../PostEffectBase.h"
#include <wrl.h>
#include <d3d12.h>


namespace CoreEngine
{
class Neon : public PostEffectBase {
public:
    /// @brief ネオンパラメータ構造体
    struct NeonParams {
        float edgeThreshold = 0.3f;     // エッジ検出の閾値 (0.0-1.0)
        float glowIntensity = 2.0f;     // グローの強度 (0.0-5.0)
        float edgeWidth = 1.0f;         // エッジの太さ (0.5-3.0)
        float colorSaturation = 1.5f;   // 色の彩度 (0.0-3.0)
        float brightness = 1.2f;        // 明るさ (0.0-3.0)
        float neonColorR = 0.0f;        // ネオンカラーR (0.0-1.0)
        float neonColorG = 1.0f;        // ネオンカラーG (0.0-1.0)
        float neonColorB = 1.0f;        // ネオンカラーB (0.0-1.0)
    };

public:
    Neon() = default;
    ~Neon() = default;

    /// @brief 初期化
    void Initialize(class DirectXCommon* dxCommon);

    /// @brief ImGuiでパラメータを調整
    void DrawImGui() override;

    /// @brief パラメータを取得
    /// @return パラメータ構造体の参照
    const NeonParams& GetParams() const { return params_; }

    /// @brief パラメータを設定して定数バッファを更新
    /// @param params 新しいパラメータ
    void SetParams(const NeonParams& params);

    /// @brief 定数バッファを強制的に更新
    void ForceUpdateConstantBuffer();

protected:
    const std::wstring& GetPixelShaderPath() const override;
    void BindOptionalCBVs(ID3D12GraphicsCommandList* commandList) override;

private:
    /// @brief 定数バッファの作成
    void CreateConstantBuffer();
    
    /// @brief 定数バッファの更新
    void UpdateConstantBuffer();

private:
    NeonParams params_;
    Microsoft::WRL::ComPtr<ID3D12Resource> constantBuffer_;
    NeonParams* mappedData_ = nullptr;
};
}
