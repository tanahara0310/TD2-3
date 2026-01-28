#include "YoYoRenderer.h"
#include "Engine/Camera/ICamera.h"
#include <cassert>

namespace CoreEngine
{
    void YoYoRenderer::Initialize(ID3D12Device* device) {

        shaderCompiler_->Initialize();

        // Root Parameter 0: マテリアル用CBV (b0, PS)
        RootSignatureManager::RootDescriptorConfig materialCBV;
        materialCBV.shaderRegister = 0;
        materialCBV.visibility = D3D12_SHADER_VISIBILITY_PIXEL;
        rootSignatureMg_->AddRootCBV(materialCBV);

        // Root Parameter 1: トランスフォーム用CBV (b0, VS)
        RootSignatureManager::RootDescriptorConfig transformCBV;
        transformCBV.shaderRegister = 0;
        transformCBV.visibility = D3D12_SHADER_VISIBILITY_VERTEX;
        rootSignatureMg_->AddRootCBV(transformCBV);

        // Root Parameter 2: カメラ用CBV (b2, PS)
        RootSignatureManager::RootDescriptorConfig cameraCBV;
        cameraCBV.shaderRegister = 2;
        cameraCBV.visibility = D3D12_SHADER_VISIBILITY_PIXEL;
        rootSignatureMg_->AddRootCBV(cameraCBV);

        rootSignatureMg_->Create(device);

        // シェーダーのコンパイルとPSO作成
        auto vertexShaderBlob = shaderCompiler_->CompileShader(L"Assets/Shaders/YoYo/YoYo.VS.hlsl", L"vs_6_0");
        assert(vertexShaderBlob != nullptr);

        auto pixelShaderBlob = shaderCompiler_->CompileShader(L"Assets/Shaders/YoYo/YoYo.PS.hlsl", L"ps_6_0");
        assert(pixelShaderBlob != nullptr);

        bool result = psoMg_->CreateBuilder()
            .AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D12_APPEND_ALIGNED_ELEMENT)
            .AddInputElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, D3D12_APPEND_ALIGNED_ELEMENT)
            .AddInputElement("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, D3D12_APPEND_ALIGNED_ELEMENT)
            .SetRasterizer(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID)
            .SetDepthStencil(true, true)
            .SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
            .BuildAllBlendModes(device, vertexShaderBlob, pixelShaderBlob, rootSignatureMg_->GetRootSignature());

        if (!result) {
            throw std::runtime_error("Failed to create YoYo Pipeline State Object");
        }

        pipelineState_ = psoMg_->GetPipelineState(BlendMode::kBlendModeNone);
    }

    void YoYoRenderer::BeginPass(ID3D12GraphicsCommandList* cmdList, BlendMode blendMode) {

        if (blendMode != currentBlendMode_) {
            currentBlendMode_ = blendMode;
            pipelineState_ = psoMg_->GetPipelineState(blendMode);
        }

        cmdList->SetGraphicsRootSignature(rootSignatureMg_->GetRootSignature());
        cmdList->SetPipelineState(pipelineState_);
        cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // カメラCBVを設定
        if (cameraCBV_ != 0) {
            cmdList->SetGraphicsRootConstantBufferView(YoYoRendererRootParam::kCamera, cameraCBV_);
        }
    }

    void YoYoRenderer::EndPass() {
    }

    void YoYoRenderer::SetCamera(const ICamera* camera) {
        if (camera) {
            cameraCBV_ = camera->GetGPUVirtualAddress();
        } else {
            cameraCBV_ = 0;
        }
    }
}
