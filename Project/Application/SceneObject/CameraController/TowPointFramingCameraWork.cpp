#include "TowPointFramingCameraWork.h"
#include "Engine/Camera/Release/Camera.h"
#include "Engine/Camera/CameraManager.h"
#include "Application/Utility/MatsumotoUtility.h"

TowPointFramingCameraWork::TowPointFramingCameraWork(
    CoreEngine::CameraManager* camera,
    CoreEngine::Vector3& point1,
    CoreEngine::Vector3& point2,
    float speed) :
    cameraManager_(camera),
    point1_(point1),
    point2_(point2),
    speed_(speed) {
}

void TowPointFramingCameraWork::Update() {
    CoreEngine::Camera* camera = static_cast<CoreEngine::Camera*>(cameraManager_->GetActiveCamera(CoreEngine::CameraType::Camera3D));
    if (!camera) return;

    // 1. 2点の中点を計算
    CoreEngine::Vector3 midPoint = (point1_ + point2_) / 2.0f;

    // 2. 2点間の距離を計算
    float distanceBetweenPoints = CoreEngine::Math::Vector::Length(point1_ - point2_);

    // 3. 必要な距離を計算
    float fovV = camera->GetParameters().fov;
    float margin = 2.5f;
    float requiredDistance = (distanceBetweenPoints * 0.5f * margin) / std::tan(fovV * 0.5f);

    // 4. カメラのforwardを取得（向きは固定）
    CoreEngine::Vector3 forward = camera->GetForward();

    // 5. カメラの目標座標を計算（向きは変えない）
    CoreEngine::Vector3 targetPos = midPoint - (forward * requiredDistance);

    // 6. カメラを滑らかに移動
    CoreEngine::Vector3 currentPos = camera->GetPosition();
    CoreEngine::Vector3 nextPos = MatsumotoUtility::SimpleEaseIn(currentPos, targetPos, speed_);
    camera->SetTranslate(nextPos);
}
