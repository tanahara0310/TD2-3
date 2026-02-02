#include "Scanline.h"
#include "Engine/Graphics/Common/DirectXCommon.h"
#include <cassert>

#ifdef _DEBUG
#include "Engine/Utility/Debug/ImGui/ImguiManager.h"
#endif

namespace CoreEngine
{

void Scanline::Initialize(DirectXCommon* dxCommon)
{
    PostEffectBase::Initialize(dxCommon);
    dxCommon_ = dxCommon;
    CreateConstantBuffer();
}

void Scanline::Update(float deltaTime)
{
    if (enabled_) {
        params_.time += deltaTime;
        UpdateConstantBuffer();
    }
}

void Scanline::DrawImGui()
{
#ifdef _DEBUG
    if (ImGui::TreeNode("Scanline Parameters")) {
        bool changed = false;

        if (ImGui::SliderFloat("Line Intensity", &params_.lineIntensity, 0.0f, 1.0f)) {
            changed = true;
        }
        ImGui::TextDisabled("走査線の強度（明るさ）");

        if (ImGui::SliderFloat("Line Width", &params_.lineWidth, 1.0f, 10.0f)) {
            changed = true;
        }
        ImGui::TextDisabled("走査線の幅（ピクセル）");

        if (ImGui::SliderFloat("Line Speed", &params_.lineSpeed, 0.0f, 200.0f)) {
            changed = true;
        }
        ImGui::TextDisabled("走査線のスクロール速度");

        if (ImGui::SliderFloat("Line Frequency", &params_.lineFrequency, 0.5f, 4.0f)) {
            changed = true;
        }
        ImGui::TextDisabled("走査線の密度");

        if (ImGui::SliderFloat("Flicker Intensity", &params_.flickerIntensity, 0.0f, 0.2f)) {
            changed = true;
        }
        ImGui::TextDisabled("フリッカー（明滅）の強度");

        if (ImGui::SliderFloat("Flicker Speed", &params_.flickerSpeed, 0.0f, 30.0f)) {
            changed = true;
        }
        ImGui::TextDisabled("フリッカーの速度");

        if (changed) {
            UpdateConstantBuffer();
        }

        ImGui::TreePop();
    }
#endif
}

void Scanline::SetParams(const ScanlineParams& params)
{
    params_ = params;
    UpdateConstantBuffer();
}

const std::wstring& Scanline::GetPixelShaderPath() const
{
    static const std::wstring path = L"Assets/Shaders/PostProcess/Scanline.PS.hlsl";
    return path;
}

void Scanline::BindOptionalCBVs(ID3D12GraphicsCommandList* commandList)
{
    if (constantBuffer_) {
        commandList->SetGraphicsRootConstantBufferView(1, constantBuffer_->GetGPUVirtualAddress());
    }
}

void Scanline::CreateConstantBuffer()
{
    if (!dxCommon_) return;

    D3D12_HEAP_PROPERTIES heapProps{};
    heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resourceDesc.Width = (sizeof(ScanlineParams) + 0xff) & ~0xff;
    resourceDesc.Height = 1;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.MipLevels = 1;
    resourceDesc.SampleDesc.Count = 1;
    resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    HRESULT hr = dxCommon_->GetDevice()->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&constantBuffer_)
    );

    assert(SUCCEEDED(hr));
    UpdateConstantBuffer();
}

void Scanline::UpdateConstantBuffer()
{
    if (!constantBuffer_) return;

    ScanlineParams* mappedData = nullptr;
    HRESULT hr = constantBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&mappedData));

    if (SUCCEEDED(hr)) {
        *mappedData = params_;
        constantBuffer_->Unmap(0, nullptr);
    }
}

} // namespace CoreEngine
