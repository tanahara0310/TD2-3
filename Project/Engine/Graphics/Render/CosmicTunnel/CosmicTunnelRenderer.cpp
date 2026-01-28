#include "CosmicTunnelRenderer.h"
#include "Engine/Camera/ICamera.h"
#include <cassert>

namespace CoreEngine
{
    void CosmicTunnelRenderer::Initialize(ID3D12Device* device) {

        shaderCompiler_->Initialize();

        // Root Parameter 0: トランスフォーム用CBV (b0, VS)
        RootSignatureManager::RootDescriptorConfig transformCBV;
        transformCBV.shaderRegister = 0;
        transformCBV.visibility = D3D12_SHADER_VISIBILITY_VERTEX;
        rootSignatureMg_->AddRootCBV(transformCBV);

        // Root Parameter 1: シーンデータ用CBV (b1, PS)
        RootSignatureManager::RootDescriptorConfig sceneCBV;
        sceneCBV.shaderRegister = 1;
        sceneCBV.visibility = D3D12_SHADER_VISIBILITY_PIXEL;
        rootSignatureMg_->AddRootCBV(sceneCBV);

        rootSignatureMg_->Create(device);

        // シェーダーのコンパイルとPSO作成
        auto vertexShaderBlob = shaderCompiler_->CompileShader(L"Assets/Shaders/CosmicTunnel/CosmicTunnel.VS.hlsl", L"vs_6_0");
        assert(vertexShaderBlob != nullptr);

        auto pixelShaderBlob = shaderCompiler_->CompileShader(L"Assets/Shaders/CosmicTunnel/CosmicTunnel.PS.hlsl", L"ps_6_0");
        assert(pixelShaderBlob != nullptr);

        bool result = psoMg_->CreateBuilder()
            .AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D12_APPEND_ALIGNED_ELEMENT)
            .AddInputElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, D3D12_APPEND_ALIGNED_ELEMENT)
            .SetRasterizer(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID)
            .SetDepthStencil(true, true)
            .SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
            .BuildAllBlendModes(device, vertexShaderBlob, pixelShaderBlob, rootSignatureMg_->GetRootSignature());

        if (!result) {
            throw std::runtime_error("Failed to create CosmicTunnel Pipeline State Object");
        }

        // 不透明描画に変更（加算ブレンドだと背景が透けてしまう）
        pipelineState_ = psoMg_->GetPipelineState(BlendMode::kBlendModeNone);
    }

    void CosmicTunnelRenderer::BeginPass(ID3D12GraphicsCommandList* cmdList, BlendMode blendMode) {

        if (blendMode != currentBlendMode_) {
            currentBlendMode_ = blendMode;
            pipelineState_ = psoMg_->GetPipelineState(blendMode);
        }

        cmdList->SetGraphicsRootSignature(rootSignatureMg_->GetRootSignature());
        cmdList->SetPipelineState(pipelineState_);
        cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // カメラCBVを設定
        if (cameraCBV_ != 0) {
            cmdList->SetGraphicsRootConstantBufferView(CosmicTunnelRendererRootParam::kScene, cameraCBV_);
        }
    }

    void CosmicTunnelRenderer::EndPass() {
    }

    void CosmicTunnelRenderer::SetCamera(const ICamera* camera) {
        if (camera) {
            cameraCBV_ = camera->GetGPUVirtualAddress();
        } else {
            cameraCBV_ = 0;
        }
    }
}
