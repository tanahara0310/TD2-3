#pragma once
#include <memory>
#include "ICameraWork.h"

namespace CoreEngine{
    class CameraManager;
}

class CameraController final{
public:
    CameraController() = delete;
    explicit CameraController(CoreEngine::CameraManager* cameraManager);
    ~CameraController() = default;

    // 初期化と更新
    void Initialize();
    void Update();

    // FOVリセット
    void ResetFov();

    // カメラワークの設定
    template<typename T, typename... Args>
    void SetCameraWork(Args&&... args) {
        cameraWork_ = std::make_unique<T>(cameraManager_,std::forward<Args>(args)...);
    }

private:
    CoreEngine::CameraManager* cameraManager_;
    std::unique_ptr<ICameraWork> cameraWork_;
};