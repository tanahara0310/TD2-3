#pragma once
#include <memory>
#include "Scene/BaseScene.h"
#include "EngineSystem/EngineSystem.h"

#include "Application/Utility/Command/SceneCommandExecutor.h"
#include "Application/SceneObject/Menu/MenuView.h"
#include "Application/SceneObject/TitleUI/TitleUIManager.h"
#include "Engine/Camera/CameraShake.h"

namespace CoreEngine {
    class CosmicTunnelObject;
    class UIFrameObject;
    class SoundManager;
    class ParticleSystem;
}

namespace CoreEngine
{
    /// @brief タイトルシーンクラス
    class TitleScene : public BaseScene {
    public:
        /// @brief 初期化
        void Initialize(EngineSystem* engine) override;

        /// @brief 描画処理
        void Draw() override;

        /// @brief 解放
        void Finalize() override;

    protected:
        /// @brief 更新処理（BaseSceneのOnUpdate()をオーバーライド）
        void OnUpdate() override;

    private:
        SceneCommandExecutor sceneCommandExecutor_;

        // UI管理
        std::unique_ptr<TitleUIManager> uiManager_;

        // 背景演出オブジェクト
        std::unique_ptr<CoreEngine::CosmicTunnelObject*> cosmicTunnel_;

        // パーティクルシステム
        CoreEngine::ParticleSystem* floatingParticles_ = nullptr;
        CoreEngine::ParticleSystem* sparkleParticles_ = nullptr;
        CoreEngine::ParticleSystem* celebrationParticles_ = nullptr;  // お祝いパーティクル（指が回転させたとき）

        // カメラシェイク
        std::unique_ptr<CoreEngine::CameraShake> cameraShake_;
        CoreEngine::Vector3 originalCamera2DPosition_;  // 2Dカメラの元の位置
        bool isShaking_ = false;  // シェイク中かどうか

        // サウンドリソース
        std::unique_ptr<CoreEngine::SoundManager::SoundResource> bgm_;
        std::unique_ptr<CoreEngine::SoundManager::SoundResource> decideSE_;

        // シーン遷移用
        float transitionProgress_ = 0.0f;
        float transitionDuration_ = 1.0f;  // トランジション時間（秒）
        bool isTransitioning_ = false;
    };
}