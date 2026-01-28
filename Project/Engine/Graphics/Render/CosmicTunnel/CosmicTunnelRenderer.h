#pragma once
#include "Engine/Graphics/Render/IRenderer.h"
#include "Engine/Graphics/PipelineStateManager.h"
#include "Engine/Graphics/RootSignatureManager.h"
#include "Engine/Graphics/Shader/ShaderCompiler.h"
#include <d3d12.h>
#include <wrl.h>
#include <memory>

namespace CoreEngine
{
namespace CosmicTunnelRendererRootParam {
    static constexpr UINT kTransform = 0;   // b0: TransformationMatrix (VS)
    static constexpr UINT kScene = 1;       // b1: SceneData (PS)
}

/// @brief CosmicTunnel描画用レンダラー
class CosmicTunnelRenderer : public IRenderer {
public:
    void Initialize(ID3D12Device* device) override;
    void BeginPass(ID3D12GraphicsCommandList* cmdList, BlendMode blendMode) override;
    void EndPass() override;
    RenderPassType GetRenderPassType() const override { return RenderPassType::Model; }
    void SetCamera(const ICamera* camera) override;
    
    ID3D12RootSignature* GetRootSignature() const { return rootSignatureMg_->GetRootSignature(); }
    
private:
    std::unique_ptr<RootSignatureManager> rootSignatureMg_ = std::make_unique<RootSignatureManager>();
    std::unique_ptr<PipelineStateManager> psoMg_ = std::make_unique<PipelineStateManager>();
    std::unique_ptr<ShaderCompiler> shaderCompiler_ = std::make_unique<ShaderCompiler>();
    
    ID3D12PipelineState* pipelineState_ = nullptr;
    BlendMode currentBlendMode_;
    D3D12_GPU_VIRTUAL_ADDRESS cameraCBV_ = 0;
};
}
