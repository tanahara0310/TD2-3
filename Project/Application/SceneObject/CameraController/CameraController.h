#pragma once
#include <memory>
#include <EngineSystem.h>
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

    // デフォルトのカメラワーク設定
    void SetDefaultCameraWork(std::unique_ptr<ICameraWork> defaultCameraWork);
    // デフォルトのカメラワークにリセット
    void ResetDefaultCameraWork();

    // カメラワークの設定
    template<typename T, typename... Args>
    void SetCameraWork(Args&&... args) {
        if (defaultCameraWork_ == nullptr && cameraWork_) {
            defaultCameraWork_ = std::move(cameraWork_);
        }
        cameraWork_ = std::make_unique<T>(cameraManager_,std::forward<Args>(args)...);
    }

private:
    CoreEngine::CameraManager* cameraManager_;
    std::unique_ptr<ICameraWork> cameraWork_;
    std::unique_ptr<ICameraWork> defaultCameraWork_;

    CoreEngine::Vector3 defaultCameraPos_;
    CoreEngine::Vector3 defaultCameraRotate_;
    float defaultCameraFov_;
};