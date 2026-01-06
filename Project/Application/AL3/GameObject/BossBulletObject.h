#pragma once

#include "Engine/ObjectCommon/GameObject.h"
#include "Engine/Utility/Timer/GameTimer.h"
#include "Math/Vector/Vector3.h"
#include "Engine/Collider/SphereCollider.h"

/// @brief ボスの弾幕弾オブジェクト
class BossBulletObject : public GameObject {
public:
    /// @brief 初期化処理
    /// @param model モデルポインタ
    /// @param texture テクスチャハンドル
    /// @param position 初期位置
    /// @param direction 発射方向（正規化済み）
    /// @param speed 弾速
    void Initialize(std::unique_ptr<Model> model, TextureManager::LoadedTexture texture, const Vector3& position, const Vector3& direction, float speed = 15.0f);

    /// @brief 更新処理
    void Update() override;

    /// @brief 描画処理
    /// @param camera カメラ
    void Draw(const ICamera* camera) override;

    /// @brief 描画パスタイプを取得
    /// @return 描画パスタイプ
    RenderPassType GetRenderPassType() const override { return RenderPassType::Model; }

    /// @brief 衝突開始イベント
    /// @param other 衝突相手のオブジェクト
    void OnCollisionEnter(GameObject* other) override;

    /// @brief コライダーを取得
    /// @return コライダーへのポインタ
    SphereCollider* GetCollider() const { return collider_.get(); }

    /// @brief 弾が範囲外に出たかチェック
    /// @return 範囲外に出た場合true
    bool IsOutOfBounds() const;

#ifdef _DEBUG
    /// @brief オブジェクト名を取得
    /// @return オブジェクト名
    const char* GetObjectName() const override { return "BossBullet"; }
#endif

    /// @brief トランスフォームを取得
    WorldTransform& GetTransform() { return transform_; }

    /// @brief モデルを取得
    Model* GetModel() { return model_.get(); }

private:
    Vector3 velocity_;                          // 移動速度ベクトル
    float speed_ = 15.0f;                       // 弾の速度（プレイヤーの弾より遅め）
    GameTimer lifetimeTimer_;                   // 寿命タイマー
    float lifetime_ = 5.0f;                     // 寿命（秒）
    std::unique_ptr<SphereCollider> collider_;  // 球形コライダー

    // アリーナ範囲制限用の定数
    static constexpr float ARENA_SIZE = 40.0f;
    static constexpr float WALL_MODEL_SIZE = 23.0f;
    static constexpr float BOUNDS_LIMIT = ARENA_SIZE - (WALL_MODEL_SIZE * 0.5f);
};
