#pragma once

#include <d3d12.h>
#include <wrl.h>
#include <memory>
#include "MathCore.h"

namespace CoreEngine {
    class DirectXCommon;
    class ResourceFactory;
}

namespace CoreEngine
{
// CosmicTunnel用のジオメトリクラス（単純な板ポリゴン）
class CosmicTunnelGeometry {
public:
    struct VertexData {
        Vector4 position;
        Vector2 texcoord;
    };

    CosmicTunnelGeometry() = default;
    ~CosmicTunnelGeometry() = default;

    void Initialize(CoreEngine::DirectXCommon* dxCommon, CoreEngine::ResourceFactory* resourceFactory);

    const D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView() const { return vertexBufferView_; }
    const D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() const { return indexBufferView_; }
    UINT GetIndexCount() const { return indexCount_; }

private:
    void CreateBuffers();
    void CreatePlane();

    CoreEngine::DirectXCommon* dxCommon_ = nullptr;
    CoreEngine::ResourceFactory* resourceFactory_ = nullptr;

    Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer_;
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

    Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer_;
    D3D12_INDEX_BUFFER_VIEW indexBufferView_{};

    VertexData* mappedVertexData_ = nullptr;
    uint32_t* mappedIndexData_ = nullptr;

    static constexpr UINT kVertexCount = 4;
    static constexpr UINT kIndexCount = 6;
    UINT vertexCount_ = kVertexCount;
    UINT indexCount_ = kIndexCount;
};
}
