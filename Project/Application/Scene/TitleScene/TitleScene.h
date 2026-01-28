#pragma once
#include "Scene/BaseScene.h"
#include "EngineSystem/EngineSystem.h"
#include "ObjectCommon/SpriteObject.h"

#include "Application/Utility/Command/SceneCommandExecutor.h"

class Player;
class Ball;
class WhiteSkyDome;
namespace CoreEngine {
    class CosmicTunnelObject;
}
#include "Application/SceneObject/Menu/MenuView.h"
#include "Application/SceneObject/Ball/BallController.h"
#include "Application/SceneObject/YoYo/YoYoObject.h"

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
        /// @brief ヨーヨー演出のアニメーション状態
        enum class YoYoAnimationState {
            Descending,      // 降下中
            Spinning,        // 高速スピン中
            Bursting,        // 火花爆発中
            Idle             // 待機
        };

        /// @brief ヨーヨー演出の更新
        void UpdateYoYoAnimation();

        SceneCommandExecutor sceneCommandExecutor_;

        Player* player_;
        Ball* ball_;
        WhiteSkyDome* skyDome_;
        std::unique_ptr<BallController> ballController_;
        CoreEngine::YoYoObject* yoyo_;

        // ヨーヨー演出用
        YoYoAnimationState yoyoState_;
        float yoyoAnimationTime_;
        float yoyoRotationSpeed_;
        Vector3 yoyoTargetPosition_;

        // タイトル画像
        CoreEngine::SpriteObject* titleSprite_;
        CoreEngine::SpriteObject* spaceStartSprite_;
        
        // 宇宙トンネル演出
        CoreEngine::CosmicTunnelObject* cosmicTunnel_;
    };
}
