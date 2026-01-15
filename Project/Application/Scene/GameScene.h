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

class Player;
class MapManager;
#include "Application/SceneObjects/CameraController/CameraController.h"
#include "Application/SceneObjects/Map/MapManager.h"
#include "Application/SceneObjects/BeatController/BeatScheduler.h"

class GameScene : public BaseScene {
public:
    GameScene();

    /// @brief 初期化
    void Initialize(EngineSystem* engine) override;
    /// @brief 更新
    void Update() override;
    /// @brief 描画処理
    void Draw() override;
    /// @brief 解放
    void Finalize() override;

private:
    Vector3 defaultCameraPos_;
    CameraController cameraController_;
    Player* player_;
    MapManager* mapManager_;
    BeatScheduler beatScheduler_;
};