#pragma once

#include "../PostEffectBase.h"
#include <wrl.h>
#include <d3d12.h>


namespace CoreEngine
{
    class BlackHole : public PostEffectBase {
    public:
        /// @brief ブラックホールエフェクトパラメータ構造体
        struct BlackHoleParams {
            float time = 0.0f;          // 時間パラメータ
            float intensity = 1.0f;     // エフェクト強度 (0.0-2.0)
            float speed = 7.0f;         // 回転速度 (1.0-20.0)
            float distortion = 2.1f;    // 歪み強度 (1.0-5.0)
        };

    public:
        BlackHole() = default;
        ~BlackHole() = default;

        /// @brief 初期化
        void Initialize(class DirectXCommon* dxCommon);

        /// @brief 更新処理
        void Update(float deltaTime) override;

        /// @brief ImGuiでパラメータを調整
        void DrawImGui() override;

        /// @brief パラメータを取得
        /// @return パラメータ構造体の参照
        const BlackHoleParams& GetParams() const { return params_; }

        /// @brief パラメータを設定して定数バッファを更新
        /// @param params 新しいパラメータ
        void SetParams(const BlackHoleParams& params);

        /// @brief 定数バッファを強制的に更新
        void ForceUpdateConstantBuffer();

    protected:
        const std::wstring& GetPixelShaderPath() const override
        {
            static const std::wstring pixelShaderPath = L"Assets/Shaders/PostProcess/BlackHole.PS.hlsl";
            return pixelShaderPath;
        }

        void BindOptionalCBVs(ID3D12GraphicsCommandList* commandList) override;

    private:
        /// @brief 定数バッファの作成
        void CreateConstantBuffer();

        /// @brief 定数バッファの更新
        void UpdateConstantBuffer();

    private:
        BlackHoleParams params_;
        Microsoft::WRL::ComPtr<ID3D12Resource> constantBuffer_;
        BlackHoleParams* mappedData_ = nullptr;
    };
}
