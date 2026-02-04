#include "TransitionEffect.h"
#include "Engine/Utility/Debug/ImGui/ImguiManager.h"
#include <cassert>

namespace CoreEngine
{
void TransitionEffect::Initialize(DirectXCommon* dxCommon)
{
    PostEffectBase::Initialize(dxCommon);
    dxCommon_ = dxCommon;
    CreateConstantBuffer();
}

const std::wstring& TransitionEffect::GetPixelShaderPath() const
{
    static const std::wstring path = L"Assets/Shaders/PostProcess/TransitionEffect.PS.hlsl";
    return path;
}

void TransitionEffect::DrawImGui()
{
#ifdef _DEBUG
    ImGui::PushID("TransitionEffectParams");
    
    ImGui::Text("状態: %s", IsEnabled() ? "有効" : "無効");
    ImGui::Separator();
    
    bool paramsChanged = false;
    
    if (ImGui::TreeNode("パラメータ")) {
        // 進行度
        if (ImGui::SliderFloat("進行度", &params_.progress, 0.0f, 1.0f)) {
            paramsChanged = true;
        }
        
        // トランジションタイプ
        const char* typeNames[] = {
            "Circle (円形)",
            "Slide (スライド)",
            "Glitch (グリッチ)",
            "Pixelate (ピクセル分解)",
            "Wipe (ワイプ)",
            "Blind (ブラインド)",
            "ZoomBlur (ズームブラー)",
            "Mosaic (モザイク)"
        };
        if (ImGui::Combo("トランジションタイプ", &params_.transitionType, typeNames, 8)) {
            paramsChanged = true;
        }
        
        ImGui::Separator();
        
        // 共通パラメータ
        if (ImGui::TreeNode("共通設定")) {
            if (ImGui::SliderFloat("中心X", &params_.centerX, 0.0f, 1.0f)) {
                paramsChanged = true;
            }
            if (ImGui::SliderFloat("中心Y", &params_.centerY, 0.0f, 1.0f)) {
                paramsChanged = true;
            }
            if (ImGui::SliderFloat("スムーズネス", &params_.smoothness, 0.0f, 0.2f)) {
                paramsChanged = true;
            }
            
            float maskColor[3] = { params_.maskColorR, params_.maskColorG, params_.maskColorB };
            if (ImGui::ColorEdit3("マスク色", maskColor)) {
                params_.maskColorR = maskColor[0];
                params_.maskColorG = maskColor[1];
                params_.maskColorB = maskColor[2];
                paramsChanged = true;
            }
            ImGui::TreePop();
        }
        
        // タイプ別パラメータ
        if (ImGui::TreeNode("タイプ別設定")) {
            switch (static_cast<TransitionType>(params_.transitionType)) {
                case TransitionType::Slide:
                    if (ImGui::SliderFloat("スライド角度", &params_.slideAngle, 0.0f, 360.0f)) {
                        paramsChanged = true;
                    }
                    break;
                    
                case TransitionType::Glitch:
                    if (ImGui::SliderFloat("グリッチ強度", &params_.glitchIntensity, 0.0f, 2.0f)) {
                        paramsChanged = true;
                    }
                    if (ImGui::SliderFloat("ランダムシード", &params_.randomSeed, 0.0f, 100.0f)) {
                        paramsChanged = true;
                    }
                    break;
                    
                case TransitionType::Pixelate:
                case TransitionType::Mosaic:
                    if (ImGui::SliderFloat("ピクセルサイズ", &params_.pixelSize, 1.0f, 100.0f)) {
                        paramsChanged = true;
                    }
                    break;
                    
                case TransitionType::Wipe:
                    if (ImGui::SliderFloat("波の周波数", &params_.waveFrequency, 0.0f, 20.0f)) {
                        paramsChanged = true;
                    }
                    if (ImGui::SliderFloat("波の振幅", &params_.waveAmplitude, 0.0f, 0.1f)) {
                        paramsChanged = true;
                    }
                    break;
                    
                case TransitionType::Blind:
                    if (ImGui::SliderFloat("ブラインド数", &params_.blindCount, 2.0f, 30.0f)) {
                        paramsChanged = true;
                    }
                    break;
                    
                default:
                    ImGui::Text("このタイプに固有の設定はありません");
                    break;
            }
            ImGui::TreePop();
        }
        
        ImGui::TreePop();
    }
    
    if (paramsChanged) {
        UpdateConstantBuffer();
    }
    
    ImGui::Separator();
    
    if (ImGui::TreeNode("クイック操作")) {
        if (ImGui::Button("リセット（開く）")) {
            params_.progress = 0.0f;
            UpdateConstantBuffer();
        }
        ImGui::SameLine();
        if (ImGui::Button("完了（閉じる）")) {
            params_.progress = 1.0f;
            UpdateConstantBuffer();
        }
        ImGui::TreePop();
    }
    
    ImGui::PopID();
#endif
}

void TransitionEffect::SetParams(const TransitionParams& params)
{
    params_ = params;
    UpdateConstantBuffer();
}

void TransitionEffect::SetProgress(float progress)
{
    params_.progress = progress;
    UpdateConstantBuffer();
}

void TransitionEffect::SetTransitionType(TransitionType type)
{
    params_.transitionType = static_cast<int>(type);
    UpdateConstantBuffer();
}

void TransitionEffect::ForceUpdateConstantBuffer()
{
    UpdateConstantBuffer();
}

void TransitionEffect::BindOptionalCBVs(ID3D12GraphicsCommandList* commandList)
{
    if (constantBuffer_) {
        commandList->SetGraphicsRootConstantBufferView(1, constantBuffer_->GetGPUVirtualAddress());
    }
}

void TransitionEffect::CreateConstantBuffer()
{
    assert(dxCommon_);

    UINT bufferSize = (sizeof(TransitionParams) + 255) & ~255;

    D3D12_HEAP_PROPERTIES heapProps{};
    heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resourceDesc.Width = bufferSize;
    resourceDesc.Height = 1;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.MipLevels = 1;
    resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
    resourceDesc.SampleDesc.Count = 1;
    resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    [[maybe_unused]] HRESULT hr = dxCommon_->GetDevice()->CreateCommittedResource(
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

void TransitionEffect::UpdateConstantBuffer()
{
    if (!constantBuffer_) return;

    // HLSL側の構造体レイアウトに合わせたバッファ構造
    struct HLSLTransitionParams {
        // float4境界 #1
        float progress;
        int transitionType;
        float centerX;
        float centerY;
        
        // float4境界 #2
        float smoothness;
        float slideAngle;
        float glitchIntensity;
        float pixelSize;
        
        // float4境界 #3
        float aspectRatio;
        float maskColorR;
        float maskColorG;
        float maskColorB;
        
        // float4境界 #4
        float blindCount;
        float randomSeed;
        float waveFrequency;
        float waveAmplitude;
    };

    HLSLTransitionParams* mappedData = nullptr;
    HRESULT hr = constantBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&mappedData));
    
    if (SUCCEEDED(hr)) {
        mappedData->progress = params_.progress;
        mappedData->transitionType = params_.transitionType;
        mappedData->centerX = params_.centerX;
        mappedData->centerY = params_.centerY;
        
        mappedData->smoothness = params_.smoothness;
        mappedData->slideAngle = params_.slideAngle;
        mappedData->glitchIntensity = params_.glitchIntensity;
        mappedData->pixelSize = params_.pixelSize;
        
        mappedData->aspectRatio = params_.aspectRatio;
        mappedData->maskColorR = params_.maskColorR;
        mappedData->maskColorG = params_.maskColorG;
        mappedData->maskColorB = params_.maskColorB;
        
        mappedData->blindCount = params_.blindCount;
        mappedData->randomSeed = params_.randomSeed;
        mappedData->waveFrequency = params_.waveFrequency;
        mappedData->waveAmplitude = params_.waveAmplitude;
        
        constantBuffer_->Unmap(0, nullptr);
    }
}

} // namespace CoreEngine
