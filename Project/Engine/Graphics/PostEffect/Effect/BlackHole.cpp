#include "BlackHole.h"
#include "Engine/Utility/Debug/ImGui/ImguiManager.h"
#include <cassert>


namespace CoreEngine
{
void BlackHole::Initialize(DirectXCommon* dxCommon)
{
    // 基底クラスの初期化
    PostEffectBase::Initialize(dxCommon);
    
    // 定数バッファの作成
    CreateConstantBuffer();
}

void BlackHole::Update(float deltaTime)
{
    // 時間を進める
    params_.time += deltaTime * params_.speed;
    UpdateConstantBuffer();
}

void BlackHole::DrawImGui()
{
#ifdef _DEBUG
    ImGui::PushID("BlackHoleParams");
    
    ImGui::Text("状態: %s", IsEnabled() ? "有効" : "無効");
    ImGui::Text("ブラックホール/渦巻きエフェクト");
    ImGui::Separator();
    
    bool paramsChanged = false;
    
    // パラメータ設定
    if (ImGui::TreeNode("パラメータ")) {
        // エフェクト強度の調整
        paramsChanged |= ImGui::SliderFloat("強度", &params_.intensity, 0.0f, 2.0f);
        
        // 回転速度の調整
        paramsChanged |= ImGui::SliderFloat("回転速度", &params_.speed, 1.0f, 20.0f);
        
        // 歪み強度の調整
        paramsChanged |= ImGui::SliderFloat("歪み強度", &params_.distortion, 1.0f, 5.0f);
        
        // 時間のリセットボタン
        if (ImGui::Button("時間をリセット")) {
            params_.time = 0.0f;
            paramsChanged = true;
        }
        
        ImGui::TreePop();
    }
    
    // パラメータが変更された場合、即座に定数バッファを更新
    if (paramsChanged) {
        UpdateConstantBuffer();
    }
    
    ImGui::Separator();
    
    if (ImGui::Button("デフォルトに戻す")) {
        params_.intensity = 1.0f;
        params_.speed = 7.0f;
        params_.distortion = 2.1f;
        params_.time = 0.0f;
        UpdateConstantBuffer();
    }
    
    if (!IsEnabled()) {
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "注意: エフェクトは無効ですが、パラメータは調整可能です");
    }
    
    ImGui::Separator();
    
    ImGui::PopID();
#endif // _DEBUG
}

void BlackHole::SetParams(const BlackHoleParams& params)
{
    params_ = params;
    UpdateConstantBuffer();
}

void BlackHole::ForceUpdateConstantBuffer()
{
    UpdateConstantBuffer();
}

void BlackHole::BindOptionalCBVs(ID3D12GraphicsCommandList* commandList)
{
    // 定数バッファをピクセルシェーダーにバインド
    if (constantBuffer_) {
        commandList->SetGraphicsRootConstantBufferView(1, constantBuffer_->GetGPUVirtualAddress());
    }
}

void BlackHole::UpdateConstantBuffer()
{
    // 定数バッファにデータをコピー
    if (mappedData_) {
        *mappedData_ = params_;
    }
}

void BlackHole::CreateConstantBuffer()
{
    assert(directXCommon_);
    
    // 定数バッファのサイズを256バイトアライメントに調整
    UINT bufferSize = (sizeof(BlackHoleParams) + 255) & ~255;
    
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
        IID_PPV_ARGS(&constantBuffer_)
    );
    assert(SUCCEEDED(hr));
    
    // マッピング
    hr = constantBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&mappedData_));
    assert(SUCCEEDED(hr));
    
    // 初期値で更新
    UpdateConstantBuffer();
}
}
