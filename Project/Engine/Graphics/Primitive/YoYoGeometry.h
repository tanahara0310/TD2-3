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
class YoYoGeometry {
public:
    struct VertexData {
        Vector4 position;
        Vector2 texcoord;
        Vector3 normal;
    };

    YoYoGeometry() = default;
    ~YoYoGeometry() = default;

    void Initialize(CoreEngine::DirectXCommon* dxCommon, CoreEngine::ResourceFactory* resourceFactory);

    const D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView() const { return vertexBufferView_; }
    const D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() const { return indexBufferView_; }
    UINT GetIndexCount() const { return indexCount_; }

private:
    void CreateBuffers();
    void CreateVertices();

    CoreEngine::DirectXCommon* dxCommon_ = nullptr;
    CoreEngine::ResourceFactory* resourceFactory_ = nullptr;

    Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer_;
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

    Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer_;
    D3D12_INDEX_BUFFER_VIEW indexBufferView_{};

    VertexData* mappedVertexData_ = nullptr;

    static constexpr UINT kSegments = 32;  // 円周方向の分割数
    static constexpr UINT kHeightSegments = 16;  // 高さ方向の分割数
    UINT vertexCount_ = 0;
    UINT indexCount_ = 0;
};
}
