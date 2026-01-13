#pragma once
#include "Scene/BaseScene.h"
// エンジンコア
#include "Engine/Audio/SoundManager.h"
#include "Engine/Camera/CameraManager.h"
#include "Engine/Camera/Debug/DebugCamera.h"
#include "Engine/Camera/Release/Camera.h"
#include "MathCore.h"
#include "Utility/Logger/Logger.h"
#include "Graphics/TextureManager.h"
#include "Engine/Graphics/Light/LightData.h"
#include "Engine/Graphics/Model/ModelManager.h"
#include "Engine/Graphics/Model/Model.h"

#include "Application/Utility/Command/SceneCommandExecutor.h"

class TitleScene : public BaseScene {
public:
    /// @brief 初期化
    void Initialize(EngineSystem* engine) override;
    /// @brief 更新
    void Update() override;
    /// @brief 描画処理
    void Draw() override;
    /// @brief 解放
    void Finalize() override;

private:
    SceneCommandExecutor frameStartCommandExecutor_;
};