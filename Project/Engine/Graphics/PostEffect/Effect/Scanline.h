#pragma once
#include "../PostEffectBase.h"
#include <wrl.h>
#include <d3d12.h>

/// @brief 走査線ポストエフェクトクラス（CRTモニター風）

namespace CoreEngine
{
class Scanline : public PostEffectBase {
public:
    /// @brief 走査線パラメータ構造体
    struct ScanlineParams {
        float lineIntensity = 0.3f;      // 線の強度 (0.0-1.0)
        float lineWidth = 2.0f;          // 線の幅（ピクセル）
        float lineSpeed = 50.0f;         // 線のスクロール速度（ピクセル/秒）
        float lineFrequency = 1.0f;      // 線の周波数（密度）
        
        float flickerIntensity = 0.05f;  // フリッカー強度
        float flickerSpeed = 10.0f;      // フリッカー速度
        float time = 0.0f;               // 経過時間
        float padding = 0.0f;            // パディング
    };

public:
    Scanline() = default;
    ~Scanline() = default;

    /// @brief 初期化
    void Initialize(class DirectXCommon* dxCommon);

    /// @brief 更新処理
    /// @param deltaTime フレーム時間
    void Update(float deltaTime);

    /// @brief ImGuiでパラメータを調整
    void DrawImGui() override;

    /// @brief パラメータを取得
    const ScanlineParams& GetParams() const { return params_; }

    /// @brief パラメータを設定
    void SetParams(const ScanlineParams& params);

protected:
    const std::wstring& GetPixelShaderPath() const override;
    void BindOptionalCBVs(ID3D12GraphicsCommandList* commandList) override;

private:
    /// @brief 定数バッファの作成
    void CreateConstantBuffer();

    /// @brief 定数バッファを更新
    void UpdateConstantBuffer();

private:
    ScanlineParams params_;
    Microsoft::WRL::ComPtr<ID3D12Resource> constantBuffer_;
    class DirectXCommon* dxCommon_ = nullptr;
};

} // namespace CoreEngine
