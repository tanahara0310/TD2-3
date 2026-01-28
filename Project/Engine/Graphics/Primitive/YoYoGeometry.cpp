#include "YoYoGeometry.h"
#include "Engine/Graphics/Common/DirectXCommon.h"
#include "Engine/Graphics/Resource/ResourceFactory.h"
#include <cassert>
#include <cmath>
#include <vector>

namespace CoreEngine
{
void YoYoGeometry::Initialize(DirectXCommon* dxCommon, ResourceFactory* resourceFactory) {
    assert(dxCommon && resourceFactory);
    dxCommon_ = dxCommon;
    resourceFactory_ = resourceFactory;

    CreateBuffers();
    CreateVertices();
}

void YoYoGeometry::CreateBuffers() {
    auto* device = dxCommon_->GetDevice();

    // 頂点数とインデックス数を計算
    vertexCount_ = (kSegments + 1) * (kHeightSegments + 1);
    indexCount_ = kSegments * kHeightSegments * 6;

    // 頂点バッファの作成
    UINT vertexBufferSize = sizeof(VertexData) * vertexCount_;
    vertexBuffer_ = resourceFactory_->CreateBufferResource(device, vertexBufferSize);

    vertexBufferView_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
    vertexBufferView_.SizeInBytes = vertexBufferSize;
    vertexBufferView_.StrideInBytes = sizeof(VertexData);

    vertexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&mappedVertexData_));

    // インデックスバッファの作成
    UINT indexBufferSize = sizeof(uint32_t) * indexCount_;
    indexBuffer_ = resourceFactory_->CreateBufferResource(device, indexBufferSize);

    indexBufferView_.BufferLocation = indexBuffer_->GetGPUVirtualAddress();
    indexBufferView_.SizeInBytes = indexBufferSize;
    indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

    uint32_t* mappedIndexData = nullptr;
    indexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&mappedIndexData));

    // インデックスデータを生成
    UINT indexOffset = 0;
    for (UINT h = 0; h < kHeightSegments; ++h) {
        for (UINT s = 0; s < kSegments; ++s) {
            UINT i0 = h * (kSegments + 1) + s;
            UINT i1 = i0 + 1;
            UINT i2 = (h + 1) * (kSegments + 1) + s;
            UINT i3 = i2 + 1;

            mappedIndexData[indexOffset++] = i0;
            mappedIndexData[indexOffset++] = i1;
            mappedIndexData[indexOffset++] = i2;

            mappedIndexData[indexOffset++] = i1;
            mappedIndexData[indexOffset++] = i3;
            mappedIndexData[indexOffset++] = i2;
        }
    }

    indexBuffer_->Unmap(0, nullptr);
}

void YoYoGeometry::CreateVertices() {
    if (!mappedVertexData_) return;

    // グリッドメッシュを生成
    // シェーダー側でプロシージャルに形状を作るため、
    // ここでは単純なグリッドメッシュを作成
    UINT vertexIndex = 0;
    for (UINT h = 0; h <= kHeightSegments; ++h) {
        float v = static_cast<float>(h) / static_cast<float>(kHeightSegments);
        
        for (UINT s = 0; s <= kSegments; ++s) {
            float u = static_cast<float>(s) / static_cast<float>(kSegments);
            
            // ダミーの位置（シェーダーで変換される）
            mappedVertexData_[vertexIndex].position = { 0.0f, 0.0f, 0.0f, 1.0f };
            mappedVertexData_[vertexIndex].texcoord = { u, v };
            mappedVertexData_[vertexIndex].normal = { 0.0f, 1.0f, 0.0f };
            
            vertexIndex++;
        }
    }
}
}
