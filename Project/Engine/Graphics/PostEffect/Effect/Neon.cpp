#include "Neon.h"
#include "Engine/Utility/Debug/ImGui/ImguiManager.h"
#include <cassert>


namespace CoreEngine
{
void Neon::Initialize(DirectXCommon* dxCommon)
{
    // 基底クラスの初期化
    PostEffectBase::Initialize(dxCommon);
    
    // 定数バッファの作成
    CreateConstantBuffer();
}

const std::wstring& Neon::GetPixelShaderPath() const
{
    static const std::wstring path = L"Assets/Shaders/PostProcess/Neon.PS.hlsl";
    return path;
}

void Neon::DrawImGui()
{
#ifdef _DEBUG
    ImGui::PushID("NeonParams");
    
    ImGui::Text("状態: %s", IsEnabled() ? "有効" : "無効");
    ImGui::Separator();
    
    bool paramsChanged = false;
    
    // パラメータ設定
    if (ImGui::TreeNode("パラメータ")) {
        // エッジ検出閾値
        if (ImGui::SliderFloat("エッジしきい値", &params_.edgeThreshold, 0.0f, 1.0f)) {
            paramsChanged = true;
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("エッジ検出の感度を調整します（低い値=より多くのエッジを検出）");
        }
        
        // グロー強度
        if (ImGui::SliderFloat("グロー強度", &params_.glowIntensity, 0.0f, 5.0f)) {
            paramsChanged = true;
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("ネオンの光の強さを調整します");
        }
        
        // エッジの太さ
        if (ImGui::SliderFloat("エッジの太さ", &params_.edgeWidth, 0.5f, 3.0f)) {
            paramsChanged = true;
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("ネオンラインの太さを調整します");
        }
        
        // 彩度
        if (ImGui::SliderFloat("彩度", &params_.colorSaturation, 0.0f, 3.0f)) {
            paramsChanged = true;
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("色の鮮やかさを調整します");
        }
        
        // 明るさ
        if (ImGui::SliderFloat("明るさ", &params_.brightness, 0.0f, 3.0f)) {
            paramsChanged = true;
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("全体の明るさを調整します");
        }
        
        ImGui::Separator();
        ImGui::Text("ネオンカラー");
        
        // ネオンカラー
        float neonColor[3] = { params_.neonColorR, params_.neonColorG, params_.neonColorB };
        if (ImGui::ColorEdit3("色", neonColor)) {
            params_.neonColorR = neonColor[0];
            params_.neonColorG = neonColor[1];
            params_.neonColorB = neonColor[2];
            paramsChanged = true;
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("ネオンの色を調整します");
        }
        
        ImGui::TreePop();
    }
    
    // パラメータが変更された場合、即座に定数バッファを更新
    if (paramsChanged) {
        UpdateConstantBuffer();
    }
    
    ImGui::Separator();
    
    // プリセット
    if (ImGui::TreeNode("プリセット")) {
        if (ImGui::Button("シアン系ネオン")) {
            params_.edgeThreshold = 0.3f;
            params_.glowIntensity = 2.5f;
            params_.edgeWidth = 1.2f;
            params_.colorSaturation = 1.8f;
            params_.brightness = 1.2f;
            params_.neonColorR = 0.0f;
            params_.neonColorG = 1.0f;
            params_.neonColorB = 1.0f;
            UpdateConstantBuffer();
        }
        
        if (ImGui::Button("マゼンタ系ネオン")) {
            params_.edgeThreshold = 0.3f;
            params_.glowIntensity = 2.5f;
            params_.edgeWidth = 1.2f;
            params_.colorSaturation = 1.8f;
            params_.brightness = 1.2f;
            params_.neonColorR = 1.0f;
            params_.neonColorG = 0.0f;
            params_.neonColorB = 1.0f;
            UpdateConstantBuffer();
        }
        
        if (ImGui::Button("黄色系ネオン")) {
            params_.edgeThreshold = 0.3f;
            params_.glowIntensity = 2.5f;
            params_.edgeWidth = 1.2f;
            params_.colorSaturation = 1.8f;
            params_.brightness = 1.2f;
            params_.neonColorR = 1.0f;
            params_.neonColorG = 1.0f;
            params_.neonColorB = 0.0f;
            UpdateConstantBuffer();
        }
        
        if (ImGui::Button("レインボー")) {
            params_.edgeThreshold = 0.25f;
            params_.glowIntensity = 3.0f;
            params_.edgeWidth = 1.5f;
            params_.colorSaturation = 2.5f;
            params_.brightness = 1.5f;
            params_.neonColorR = 0.5f;
            params_.neonColorG = 0.5f;
            params_.neonColorB = 0.5f;
            UpdateConstantBuffer();
        }
        
        if (ImGui::Button("デフォルトに戻す")) {
            params_.edgeThreshold = 0.3f;
            params_.glowIntensity = 2.0f;
            params_.edgeWidth = 1.0f;
            params_.colorSaturation = 1.5f;
            params_.brightness = 1.2f;
            params_.neonColorR = 0.0f;
            params_.neonColorG = 1.0f;
            params_.neonColorB = 1.0f;
            UpdateConstantBuffer();
        }
        
        ImGui::TreePop();
    }
    
    if (!IsEnabled()) {
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "注意: エフェクトは無効ですが、パラメータは調整可能です");
    }
    
    ImGui::PopID();
#endif // _DEBUG
}

void Neon::SetParams(const NeonParams& params)
{
    params_ = params;
    UpdateConstantBuffer();
}

void Neon::ForceUpdateConstantBuffer()
{
    UpdateConstantBuffer();
}

void Neon::BindOptionalCBVs(ID3D12GraphicsCommandList* commandList)
{
    // 定数バッファをピクセルシェーダーにバインド
    if (constantBuffer_) {
        commandList->SetGraphicsRootConstantBufferView(1, constantBuffer_->GetGPUVirtualAddress());
    }
}

void Neon::UpdateConstantBuffer()
{
    // 定数バッファにデータをコピー
    if (mappedData_) {
        *mappedData_ = params_;
    }
}

void Neon::CreateConstantBuffer()
{
    assert(directXCommon_);
    
    // 定数バッファのサイズを256バイトアライメントに調整
    UINT bufferSize = (sizeof(NeonParams) + 255) & ~255;
    
    // ヒーププロパティ
    D3D12_HEAP_PROPERTIES heapProps = {};
    heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
    heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    
    // リソースデスク
    D3D12_RESOURCE_DESC resourceDesc = {};
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resourceDesc.Width = bufferSize;
    resourceDesc.Height = 1;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.MipLevels = 1;
    resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
    resourceDesc.SampleDesc.Count = 1;
    resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
    
    // リソースの作成
    HRESULT hr = directXCommon_->GetDevice()->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&constantBuffer_));
    
    assert(SUCCEEDED(hr));
    
    // マッピング
    hr = constantBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&mappedData_));
    assert(SUCCEEDED(hr));
    
    // 初期値を設定
    UpdateConstantBuffer();
}
}
