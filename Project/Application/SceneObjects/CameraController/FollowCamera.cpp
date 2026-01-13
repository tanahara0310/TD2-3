#include "FollowCamera.h"
#include "Engine/Camera/CameraManager.h"
#include "Engine/Camera/Release/Camera.h"

#include "Application/Utility/MatsumotoUtility.h"

FollowCamera::FollowCamera(CameraManager& camera, Vector3& followPos, const Vector3& offset, float speed)
    :cameraManager_(camera), followPos_(followPos), offset_(offset), speed_(speed) {}

void FollowCamera::Update() {
    Camera* camera = static_cast<Camera*>(cameraManager_.GetActiveCamera(CameraType::Camera3D));
    if (!camera) {
        return;
    }
    Vector3 cameraPos = MatsumotoUtility::SimpleEaseIn(camera->GetPosition(), followPos_+ offset_, speed_);
    camera->SetTranslate(cameraPos);
}
