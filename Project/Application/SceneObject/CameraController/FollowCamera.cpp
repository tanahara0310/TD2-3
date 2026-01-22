#include "FollowCamera.h"
#include "Engine/Camera/Release/Camera.h"
#include "Engine/Camera/CameraManager.h"
#include "Application/Utility/MatsumotoUtility.h"

FollowCamera::FollowCamera(CoreEngine::CameraManager* camera, CoreEngine::Vector3& followPos, const CoreEngine::Vector3& offset, float speed)
    :cameraManager_(camera), followPos_(followPos), offset_(offset), speed_(speed) {}

void FollowCamera::Update() {
    CoreEngine::Camera* camera = static_cast<CoreEngine::Camera*>(cameraManager_->GetActiveCamera(CoreEngine::CameraType::Camera3D));
    if (!camera) {
        return;
    }
    CoreEngine::Vector3 cameraPos = MatsumotoUtility::SimpleEaseIn(camera->GetPosition(), followPos_+ offset_, speed_);
    camera->SetTranslate(cameraPos);
}
