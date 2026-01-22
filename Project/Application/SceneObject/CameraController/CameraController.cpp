#include "CameraController.h"
#include "Engine/Camera/CameraManager.h"
#include "Engine/Camera/Release/Camera.h"

CameraController::CameraController(CoreEngine::CameraManager* cameraManager) :
    cameraManager_(cameraManager),
    cameraWork_(nullptr) {}

void CameraController::Initialize() {
    if (cameraWork_) {
        cameraWork_.reset();
        cameraWork_ = nullptr;
    }
}

void CameraController::Update() {
    if (cameraWork_) {
        cameraWork_->Update();
    }
}

void CameraController::ResetFov() {
    CoreEngine::Camera* camera = static_cast<CoreEngine::Camera*>(cameraManager_->GetActiveCamera(CoreEngine::CameraType::Camera3D));
    CoreEngine::CameraParameters params = camera->GetParameters();
    params.fov = 0.45f;
    camera->SetParameters(params);
}