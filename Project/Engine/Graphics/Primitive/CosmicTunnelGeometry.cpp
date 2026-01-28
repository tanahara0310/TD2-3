#include "CosmicTunnelGeometry.h"
#include "Engine/Graphics/Common/DirectXCommon.h"
#include "Engine/Graphics/Resource/ResourceFactory.h"

namespace CoreEngine
{

void CosmicTunnelGeometry::Initialize(CoreEngine::DirectXCommon* dxCommon, CoreEngine::ResourceFactory* resourceFactory)
{
    dxCommon_ = dxCommon;
    resourceFactory_ = resourceFactory;

    CreateBuffers();
    CreatePlane();
}

void CosmicTunnelGeometry::CreateBuffers()
{
    // 頂点バッファの作成
    vertexBuffer_ = resourceFactory_->CreateBufferResource(
        dxCommon_->GetDevice(),
        sizeof(VertexData) * kVertexCount
    );

    vertexBufferView_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
    vertexBufferView_.SizeInBytes = sizeof(VertexData) * kVertexCount;
    vertexBufferView_.StrideInBytes = sizeof(VertexData);

    vertexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&mappedVertexData_));

    // インデックスバッファの作成
    indexBuffer_ = resourceFactory_->CreateBufferResource(
        dxCommon_->GetDevice(),
        sizeof(uint32_t) * kIndexCount
    );

    indexBufferView_.BufferLocation = indexBuffer_->GetGPUVirtualAddress();
    indexBufferView_.SizeInBytes = sizeof(uint32_t) * kIndexCount;
    indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

    indexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&mappedIndexData_));
}

void CosmicTunnelGeometry::CreatePlane()
{
    // 板ポリゴンの頂点を作成（カメラに対して正面を向く）
    // シェーダー側でレイマーチングするので、単純な矩形でOK
    float size = 10.0f; // 大きめに作成
    
    // 頂点データ
    mappedVertexData_[0] = { { -size, -size, 0.0f, 1.0f }, { 0.0f, 1.0f } }; // 左下
    mappedVertexData_[1] = { { -size,  size, 0.0f, 1.0f }, { 0.0f, 0.0f } }; // 左上
    mappedVertexData_[2] = { {  size, -size, 0.0f, 1.0f }, { 1.0f, 1.0f } }; // 右下
    mappedVertexData_[3] = { {  size,  size, 0.0f, 1.0f }, { 1.0f, 0.0f } }; // 右上

    // インデックスデータ（時計回り）
    mappedIndexData_[0] = 0;
    mappedIndexData_[1] = 1;
    mappedIndexData_[2] = 2;
    mappedIndexData_[3] = 1;
    mappedIndexData_[4] = 3;
    mappedIndexData_[5] = 2;
}

}
