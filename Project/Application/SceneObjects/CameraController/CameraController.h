#pragma once
#include <memory>
#include "Application/SceneObjects/CameraController/ICameraWork.h"

class CameraController final{
public:
    CameraController() = default;
    ~CameraController() = default;

    // 初期化と更新
    void Initialize();
    void Update();

    // カメラワークの設定
    template<typename T, typename... Args>
    void SetCameraWork(Args&&... args) {
        cameraWork_ = std::make_unique<T>(std::forward<Args>(args)...);
    }

private:
    std::unique_ptr<ICameraWork> cameraWork_;
};