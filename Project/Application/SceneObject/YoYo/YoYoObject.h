#pragma once
#include <memory>
#include "Engine/ObjectCommon/GameObject.h"
#include "Engine/Graphics/Primitive/YoYoGeometry.h"
#include "Engine/Graphics/Render/YoYo/YoYoRenderer.h"
#include "Engine/Graphics/Structs/Material.h"

namespace CoreEngine
{

class YoYoObject final : public GameObject {
public:
    YoYoObject();
    void Initialize();
    void Update() override;
    void Draw(const ICamera* camera) override;
    
    RenderPassType GetRenderPassType() const override { return RenderPassType::Model; }
    
#ifdef _DEBUG
    const char* GetObjectName() const override { return "YoYoObject"; }
#endif

    void SetRotation(const Vector3& rotation) { transform_.rotate = rotation; }
    void SetPosition(const Vector3& position) { transform_.translate = position; }
    void SetScale(const Vector3& scale) { transform_.scale = scale; }
    void SetRotationSpeed(float speed) { rotationSpeed_ = speed; }
    void SetSparkIntensity(float intensity) { sparkIntensity_ = intensity; }
    
    Vector3 GetPosition() const { return transform_.translate; }

private:
    std::unique_ptr<YoYoGeometry> geometry_;
    std::unique_ptr<YoYoRenderer> renderer_;
    
    Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
    Microsoft::WRL::ComPtr<ID3D12Resource> transformResource_;
    Microsoft::WRL::ComPtr<ID3D12Resource> cameraResource_;
    
    struct YoYoMaterialData {
        Vector4 color;
        int enableLighting;
        float time;
        float sparkIntensity;
        float padding;
    };
    
    YoYoMaterialData* materialData_ = nullptr;
    
    struct TransformationMatrix {
        Matrix4x4 WVP;
        Matrix4x4 World;
        Matrix4x4 WorldInversTranspose;
    };
    
    TransformationMatrix* transformData_ = nullptr;
    
    struct CameraData {
        Vector3 worldPosition;
        float padding;
    };
    
    CameraData* cameraData_ = nullptr;
    
    float rotationSpeed_ = 1.0f;
    float elapsedTime_ = 0.0f;
    float sparkIntensity_ = 0.0f;
};

}
