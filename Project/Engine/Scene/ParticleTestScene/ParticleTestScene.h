#pragma once

#include <memory>

// シーン関連
#include "Scene/BaseScene.h"
#include "EngineSystem/EngineSystem.h"

// パーティクルシステム
#include "Engine/Particle/ParticleSystem.h"

// 数字表示ユーティリティ
#include "Engine/Utility/NumberDisplay/NumberDisplayUtility.h"

/// @brief パーティクルテスト専用シーンクラス

namespace CoreEngine
{
    class ParticleTestScene : public BaseScene {
    public:
        /// @brief 初期化
        void Initialize(EngineSystem* engine) override;

        /// @brief 描画
        void Draw() override;

        /// @brief 解放
        void Finalize() override;

    protected:
        /// @brief 更新処理（BaseSceneのOnUpdate()をオーバーライド）
        void OnUpdate() override;

    private:
        // パーティクルシステム（テスト用）
        ParticleSystem* particleSystem_ = nullptr;

        // 敵死亡時パーティクルシステム
        ParticleSystem* enemyDeathParticle_ = nullptr;

        // 敵リスポーン時パーティクルシステム（パーティクルが中心に集まる）
        ParticleSystem* enemyRespawnParticle_ = nullptr;

        // 数字表示（使用例）
        std::unique_ptr<NumberDisplayUtility> numberDisplay_;
        int testScore_ = 0;

        // テスト用のタイマー（パーティクル再生制御用）
        float deathParticleTimer_ = 0.0f;
        float respawnParticleTimer_ = 0.0f;
        const float kParticleInterval_ = 5.0f; // 5秒ごとに再生
    };
}
