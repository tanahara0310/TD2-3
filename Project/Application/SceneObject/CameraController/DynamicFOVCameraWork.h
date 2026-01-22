#pragma once
#include "ICameraWork.h"
#include "Engine/EngineSystem/EngineSystem.h"

class DynamicFOVCameraWork final : public ICameraWork {
public:
    DynamicFOVCameraWork() = delete;
    explicit DynamicFOVCameraWork(
        CoreEngine::CameraManager* camera,
        CoreEngine::Vector3& targetPos,
        float speed);
    ~DynamicFOVCameraWork() override = default;
    void Update() override;
private:
    const float speed_;
    const CoreEngine::Vector3 targetFOV_;
    CoreEngine::CameraManager* cameraManager_;
};