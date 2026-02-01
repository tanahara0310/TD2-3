#include "RetroGrid.h"
#include "Engine/Utility/Debug/ImGui/ImguiManager.h"
#include "Engine/Graphics/Common/DirectXCommon.h"
#include <cassert>

namespace CoreEngine
{
void RetroGrid::Initialize(DirectXCommon* dxCommon)
{
    PostEffectBase::Initialize(dxCommon);
    CreateConstantBuffer();
}

const std::wstring& RetroGrid::GetPixelShaderPath() const
{
    static const std::wstring path = L"Assets/Shaders/PostProcess/RetroGrid.PS.hlsl";
    return path;
}

void RetroGrid::Update(float deltaTime)
{
    params_.time += deltaTime;
    UpdateConstantBuffer();
}

void RetroGrid::DrawImGui()
{
#ifdef _DEBUG
    ImGui::PushID("RetroGridParams");
    
    ImGui::Text("状態: %s", IsEnabled() ? "有効" : "無効");
    ImGui::Separator();
    
    bool paramsChanged = false;
    
    if (ImGui::TreeNode("グリッドパラメータ")) {
        if (ImGui::SliderFloat("グリッドサイズ", &params_.gridSize, 10.0f, 50.0f)) {
            paramsChanged = true;
        }
        
        if (ImGui::SliderFloat("ライン太さ", &params_.lineWidth, 0.01f, 0.2f)) {
            paramsChanged = true;
        }
        
        if (ImGui::SliderFloat("グリッド透明度", &params_.gridAlpha, 0.0f, 1.0f)) {
            paramsChanged = true;
        }
        
        if (ImGui::SliderFloat("パースの強度", &params_.perspectiveStrength, 0.0f, 1.0f)) {
            paramsChanged = true;
        }
        
        ImGui::TreePop();
    }
    
    if (ImGui::TreeNode("エフェクトパラメータ")) {
        if (ImGui::SliderFloat("スキャンライン速度", &params_.scanlineSpeed, 0.0f, 5.0f)) {
            paramsChanged = true;
        }
        
        if (ImGui::SliderFloat("スキャンライン強度", &params_.scanlineIntensity, 0.0f, 1.0f)) {
            paramsChanged = true;
        }
        
        if (ImGui::SliderFloat("地平線グロー", &params_.horizonGlow, 0.0f, 2.0f)) {
            paramsChanged = true;
        }
        
        if (ImGui::SliderFloat("ノイズ強度", &params_.noiseIntensity, 0.0f, 1.0f)) {
            paramsChanged = true;
        }
        
        ImGui::TreePop();
    }
    
    if (ImGui::TreeNode("カラー設定")) {
        float gridColor[3] = { params_.gridColorR, params_.gridColorG, params_.gridColorB };
        if (ImGui::ColorEdit3("グリッドカラー", gridColor)) {
            params_.gridColorR = gridColor[0];
            params_.gridColorG = gridColor[1];
            params_.gridColorB = gridColor[2];
            paramsChanged = true;
        }
        
        ImGui::TreePop();
    }
    
    if (ImGui::TreeNode("プリセット")) {
        if (ImGui::Button("シアングリッド")) {
            params_.gridSize = 20.0f;
            params_.lineWidth = 0.05f;
            params_.gridColorR = 0.0f;
            params_.gridColorG = 1.0f;
            params_.gridColorB = 1.0f;
            params_.gridAlpha = 0.5f;
            params_.horizonGlow = 0.8f;
            paramsChanged = true;
        }
        
        if (ImGui::Button("マゼンタグリッド")) {
            params_.gridSize = 20.0f;
            params_.lineWidth = 0.05f;
            params_.gridColorR = 1.0f;
            params_.gridColorG = 0.0f;
            params_.gridColorB = 1.0f;
            params_.gridAlpha = 0.5f;
            params_.horizonGlow = 0.8f;
            paramsChanged = true;
        }
        
        if (ImGui::Button("細かいグリッド")) {
            params_.gridSize = 30.0f;
            params_.lineWidth = 0.03f;
            params_.perspectiveStrength = 0.8f;
            paramsChanged = true;
        }
        
        if (ImGui::Button("デフォルト")) {
            params_.gridSize = 20.0f;
            params_.lineWidth = 0.05f;
            params_.scanlineSpeed = 1.0f;
            params_.scanlineIntensity = 0.3f;
            params_.gridColorR = 0.0f;
            params_.gridColorG = 1.0f;
            params_.gridColorB = 1.0f;
            params_.gridAlpha = 0.5f;
            params_.horizonGlow = 0.8f;
            params_.perspectiveStrength = 0.6f;
            params_.noiseIntensity = 0.1f;
            paramsChanged = true;
        }
        
        ImGui::TreePop();
    }
    
    if (paramsChanged) {
        UpdateConstantBuffer();
    }
    
    ImGui::PopID();
#endif
}

void RetroGrid::SetParams(const RetroGridParams& params)
{
    params_ = params;
    UpdateConstantBuffer();
}

void RetroGrid::BindOptionalCBVs(ID3D12GraphicsCommandList* commandList)
{
    if (constantBuffer_) {
        commandList->SetGraphicsRootConstantBufferView(1, constantBuffer_->GetGPUVirtualAddress());
    }
}

void RetroGrid::CreateConstantBuffer()
{
    HRESULT result;
    
    D3D12_HEAP_PROPERTIES heapProps{};
    heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
    
    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resourceDesc.Width = (sizeof(RetroGridParams) + 0xff) & ~0xff;
    resourceDesc.Height = 1;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.MipLevels = 1;
    resourceDesc.SampleDesc.Count = 1;
    resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    
    result = directXCommon_->GetDevice()->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&constantBuffer_));
    
    assert(SUCCEEDED(result));
    
    result = constantBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&mappedData_));
    assert(SUCCEEDED(result));
    
    UpdateConstantBuffer();
}

void RetroGrid::UpdateConstantBuffer()
{
    if (mappedData_) {
        *mappedData_ = params_;
    }
}
}
