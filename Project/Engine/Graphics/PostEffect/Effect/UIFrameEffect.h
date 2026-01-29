#pragma once

#include "../PostEffectBase.h"
#include <wrl.h>
#include <d3d12.h>

namespace CoreEngine
{
/// @brief UIフレーム描画用ポストエフェクト
class UIFrameEffect : public PostEffectBase {
public:
    /// @brief UIFrameパラメータ構造体
    struct UIFrameParams {
        float time = 0.0f;      // 経過時間
        float padding1 = 0.0f;
        float padding2 = 0.0f;
        float padding3 = 0.0f;
    };

public:
    UIFrameEffect() = default;
    ~UIFrameEffect() = default;

    /// @brief 初期化
    void Initialize(DirectXCommon* dxCommon) override;

    /// @brief 更新処理
    void Update(float deltaTime) override;

    /// @brief ImGuiでパラメータを調整
    void DrawImGui() override;

    /// @brief パラメータを取得
    const UIFrameParams& GetParams() const { return params_; }

    /// @brief パラメータを設定
    void SetParams(const UIFrameParams& params);

protected:
    const std::wstring& GetPixelShaderPath() const override
    {
        static const std::wstring pixelShaderPath = L"Assets/Shaders/PostProcess/UIFrame.PS.hlsl";
        return pixelShaderPath;
    }

    void BindOptionalCBVs(ID3D12GraphicsCommandList* commandList) override;

private:
    /// @brief 定数バッファの作成
    void CreateConstantBuffer();
    
    /// @brief 定数バッファの更新
    void UpdateConstantBuffer();

private:
    UIFrameParams params_;
    Microsoft::WRL::ComPtr<ID3D12Resource> constantBuffer_;
    UIFrameParams* mappedData_ = nullptr;
};
}
