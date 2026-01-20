#include "CameraController.h"
#include "Engine/Camera/CameraManager.h"

void CameraController::Initialize() {
    if (cameraWork_) {
        cameraWork_ = nullptr;
    }
}

void CameraController::Update() {
    if (cameraWork_) {
        cameraWork_->Update();
    }
}


