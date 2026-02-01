#pragma once

/// @brief 描画パスタイプ

namespace CoreEngine
{
    enum class RenderPassType {
        Invalid = -1,        // 無効
        Model = 0,           // 通常モデル
        SkinnedModel,        // スキニングモデル
        SkyBox,              // SkyBox
        ModelParticle,       // モデルパーティクル（3D）
        Line,                // ライン描画（デバッグ用）
        Sprite,              // スプライト（最前面）
        Particle,            // パーティクル（ビルボード）
        Text,                // テキスト描画
    };
}
