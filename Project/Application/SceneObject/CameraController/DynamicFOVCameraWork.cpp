#include "DynamicFOVCameraWork.h"
#include "Engine/Camera/Release/Camera.h"
#include "Engine/Camera/CameraManager.h"
#include "Application/Utility/MatsumotoUtility.h"

DynamicFOVCameraWork::DynamicFOVCameraWork(
    CoreEngine::CameraManager* camera,
    CoreEngine::Vector3& targetPos, 
    float speed):
    cameraManager_(camera),
    targetFOV_(targetPos),
    speed_(speed) {
}

void DynamicFOVCameraWork::Update() {
    CoreEngine::Camera* camera = static_cast<CoreEngine::Camera*>(cameraManager_->GetActiveCamera(CoreEngine::CameraType::Camera3D));
    if (!camera) {
        return;
    }
    CoreEngine::Vector3 direction = targetFOV_ - camera->GetPosition();
    float distance = CoreEngine::Math::Vector::Length(direction);

    // ここでターゲットの高さを指定（例: 2.0f）
    float targetHeight = 2.0f;

    // 垂直FOVを計算
    float desiredFOV = 2.0f * atan((targetHeight * 0.5f) / distance);
    // FOVの上限をπ（3.14159...）に制限
    desiredFOV = std::clamp(desiredFOV, 0.1f, 3.14159f);

    float currentFOV = camera->GetParameters().fov;
    float newFOV = MatsumotoUtility::SimpleEaseIn(currentFOV, desiredFOV, speed_);
    CoreEngine::CameraParameters params = camera->GetParameters();
    params.fov = newFOV;
    camera->SetParameters(params);
}
