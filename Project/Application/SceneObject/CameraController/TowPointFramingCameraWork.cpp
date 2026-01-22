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

    // 1. 2点の中点（注視点）を計算
    CoreEngine::Vector3 midPoint = (point1_ + point2_) / 2.0f;

    // 2. 2点間の距離を計算
    float distanceBetweenPoints = CoreEngine::Math::Vector::Length(point1_ - point2_);

    // 3. 2点を収めるために必要な「引きの距離」を計算
    // FOV（垂直画角）を使って、三角形の計算で距離を出します
    float fovV = camera->GetParameters().fov; // ラジアン想定
    // 距離 = (被写体の幅 / 2) / tan(画角 / 2)
    // 余裕を持たせるためにマージン（1.2f）を乗算
    float margin = 1.2f;
    float requiredDistance = (distanceBetweenPoints * 0.5f * margin) / std::tan(fovV * 0.5f);

    // 4. カメラの目標座標を計算
    // カメラの前方ベクトル(Forward)の逆方向に、計算した距離分だけ中点から離す
    CoreEngine::Vector3 forward = camera->GetForward();
    CoreEngine::Vector3 targetPos = midPoint - (forward * requiredDistance);

    // 5. 滑らかに移動させる
    CoreEngine::Vector3 currentPos = camera->GetPosition();
    CoreEngine::Vector3 nextPos = MatsumotoUtility::SimpleEaseIn(currentPos, targetPos, speed_);

    camera->SetTranslate(nextPos);

    // 6. 中点を常に見るように回転をセット（LookAt）
    camera->LookAt(midPoint);
}
