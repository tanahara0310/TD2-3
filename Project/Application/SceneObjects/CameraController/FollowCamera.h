#pragma once
#include "ICameraWork.h"
#include "Engine/Math/Vector/Vector3.h"

class CameraManager;

class FollowCamera : public ICameraWork {
public:
    FollowCamera() = delete;
    explicit FollowCamera(CameraManager& camera,Vector3& followPos, const Vector3& offset,float speed);
    ~FollowCamera() override = default;

    void Update() override;
private:
    const float speed_;
    Vector3& followPos_;
    const Vector3& offset_;
    CameraManager& cameraManager_;

};