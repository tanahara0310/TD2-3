#pragma once
#include <memory>
#include "Engine/ObjectCommon/GameObject.h"
#include "Engine/Graphics/Primitive/CosmicTunnelGeometry.h"
#include "Engine/Graphics/Render/CosmicTunnel/CosmicTunnelRenderer.h"
#include "MathCore.h"

namespace CoreEngine
{

class CosmicTunnelObject final : public GameObject {
public:
    CosmicTunnelObject();
    void Initialize();
    void Update() override;
    void Draw(const ICamera* camera) override;
    
    RenderPassType GetRenderPassType() const override { return RenderPassType::Model; }
    
#ifdef _DEBUG
    const char* GetObjectName() const override { return "CosmicTunnelObject"; }
#endif

    void SetPosition(const Vector3& position) { transform_.translate = position; }
    void SetRotation(const Vector3& rotation) { transform_.rotate = rotation; }
    void SetScale(const Vector3& scale) { transform_.scale = scale; }
    void SetIntensity(float intensity) { intensity_ = intensity; }
    
    Vector3 GetPosition() const { return transform_.translate; }
    float GetIntensity() const { return intensity_; }

private:
    std::unique_ptr<CosmicTunnelGeometry> geometry_;
    std::unique_ptr<CosmicTunnelRenderer> renderer_;
    
    Microsoft::WRL::ComPtr<ID3D12Resource> transformResource_;
    Microsoft::WRL::ComPtr<ID3D12Resource> sceneDataResource_;
    
    struct TransformationMatrix {
        Matrix4x4 WVP;
        Matrix4x4 World;
        Matrix4x4 ViewInverse;
        Vector4 cameraPosition;
    };
    
    struct SceneData {
        float time;
        Vector3 cameraPosition;
        Vector2 resolution;
        float intensity;
        float padding;
    };
    
    TransformationMatrix* transformData_ = nullptr;
    SceneData* sceneData_ = nullptr;
    
    float elapsedTime_ = 0.0f;
    float intensity_ = 1.0f;
};

}
