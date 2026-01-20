#pragma once
#include "ICameraWork.h"
#include "Engine/EngineSystem/EngineSystem.h"

namespace CoreEngine {
    class CameraManager;
}

class FollowCamera : public ICameraWork {
public:
    FollowCamera() = delete;
    explicit FollowCamera(CoreEngine::CameraManager& camera, CoreEngine::Vector3& followPos, const CoreEngine::Vector3& offset,float speed);
    ~FollowCamera() override = default;

    void Update() override;
private:
    const float speed_;
    CoreEngine::Vector3& followPos_;
    const CoreEngine::Vector3& offset_;
    CoreEngine::CameraManager& cameraManager_;

};