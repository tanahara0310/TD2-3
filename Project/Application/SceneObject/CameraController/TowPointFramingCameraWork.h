#pragma once
#include "ICameraWork.h"
#include "Engine/EngineSystem/EngineSystem.h"

class TowPointFramingCameraWork final : public ICameraWork {
public:
    TowPointFramingCameraWork() = delete;
    explicit TowPointFramingCameraWork(
        CoreEngine::CameraManager* camera,
        CoreEngine::Vector3& point1,
        CoreEngine::Vector3& point2,
        float speed);
    ~TowPointFramingCameraWork() override = default;

    void Update() override;

private:
    const float speed_;
    CoreEngine::Vector3 point1_;
    CoreEngine::Vector3 point2_;
    CoreEngine::CameraManager* cameraManager_;
};