#include "GoToCamera.h"
#include "Engine/Camera/Release/Camera.h"
#include "Engine/Camera/CameraManager.h"
#include "Application/Utility/MatsumotoUtility.h"

GoToCamera::GoToCamera(CoreEngine::CameraManager* camera, const CoreEngine::Vector3& targetPos, float speed) :
    cameraManager_(camera),
    targetPos_(targetPos),
    speed_(speed) {
}

void GoToCamera::Update() {
    CoreEngine::Camera* camera = static_cast<CoreEngine::Camera*>(cameraManager_->GetActiveCamera(CoreEngine::CameraType::Camera3D));
    if (!camera) {
        return;
    }
    CoreEngine::Vector3 cameraPos = MatsumotoUtility::SimpleEaseIn(camera->GetPosition(), targetPos_, speed_);
    camera->SetTranslate(cameraPos);
}
