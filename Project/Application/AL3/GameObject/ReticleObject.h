#pragma once

#include "Engine/ObjectCommon/GameObject.h"
#include "Engine/ObjectCommon/SpriteObject.h"
#include "Engine/Math/Vector/Vector2.h"

/// @brief レティクル（照準）オブジェクト
class ReticleObject : public GameObject {
public:
    /// @brief デストラクタ
    ~ReticleObject() override;

    /// @brief 初期化処理
    /// @param textureFilePath テクスチャファイルパス
    void Initialize(const std::string& textureFilePath);

    /// @brief 更新処理
    void Update() override;

    /// @brief 描画処理（スプライトが自動的に描画される）
    /// @param camera カメラ
    void Draw(const ICamera* camera) override;

    /// @brief 描画パスタイプを取得
    /// @return 描画パスタイプ（Sprite）
    RenderPassType GetRenderPassType() const override { return RenderPassType::Sprite; }

    /// @brief ブレンドモードを取得（スプライトのブレンドモードを返す）
    /// @return ブレンドモード
    BlendMode GetBlendMode() const override { 
        return sprite_ ? sprite_->GetBlendMode() : BlendMode::kBlendModeNormal; 
    }

    /// @brief ブレンドモードを設定（スプライトに委譲）
    /// @param blendMode ブレンドモード
    void SetBlendMode(BlendMode blendMode) override { 
        if (sprite_) {
            sprite_->SetBlendMode(blendMode);
        }
    }

    /// @brief レティクルの正規化画面座標を取得（-1.0～1.0の範囲）
    /// @return 正規化座標（中央が0,0）
    Vector2 GetNormalizedPosition() const { return normalizedPosition_; }

    /// @brief レティクルの移動速度を設定
    /// @param speed 移動速度（ピクセル/秒）
    void SetMoveSpeed(float speed) { moveSpeed_ = speed; }

    /// @brief レティクルの移動速度を取得
    /// @return 移動速度（ピクセル/秒）
    float GetMoveSpeed() const { return moveSpeed_; }

    /// @brief レティクルの移動範囲を設定（画面端からの余白）
    /// @param marginX X軸方向の余白（ピクセル）
    /// @param marginY Y軸方向の余白（ピクセル）
    void SetMoveMargin(float marginX, float marginY) { 
        marginX_ = marginX; 
        marginY_ = marginY; 
    }

private:
    /// @brief 入力処理
    /// @param deltaTime デルタタイム
    void ProcessInput(float deltaTime);

    /// @brief 移動方向を計算
    /// @return 移動方向ベクトル
    Vector2 CalculateMoveDirection() const;

    /// @brief レティクルを移動
    /// @param direction 移動方向
    /// @param deltaTime デルタタイム
    void MoveReticle(const Vector2& direction, float deltaTime);

    /// @brief 画面座標を正規化座標に変換
    void UpdateNormalizedPosition();

    // スプライトオブジェクト
    SpriteObject* sprite_ = nullptr;

    // 正規化座標（-1.0～1.0の範囲、中央が0,0）
    Vector2 normalizedPosition_ = { 0.0f, 0.0f };

    // 移動速度（ピクセル/秒）
    float moveSpeed_ = 300.0f;

    // 移動範囲の余白（画面端からの距離）
    float marginX_ = 50.0f;
    float marginY_ = 50.0f;
};
