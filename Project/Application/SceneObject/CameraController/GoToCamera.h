#pragma once
#include "ICameraWork.h"
#include "Engine/EngineSystem/EngineSystem.h"
namespace CoreEngine {
    class CameraManager;
}
class GoToCamera final : public ICameraWork {
public:
    GoToCamera() = delete;
    explicit GoToCamera(
        CoreEngine::CameraManager* camera,
        const CoreEngine::Vector3& targetPos,
        float speed);
    ~GoToCamera() override = default;
    void Update() override;
private:
    const float speed_;
    const CoreEngine::Vector3 targetPos_;
    CoreEngine::CameraManager* cameraManager_;
};