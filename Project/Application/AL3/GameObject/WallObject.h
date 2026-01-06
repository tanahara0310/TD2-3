#pragma once

#include "Engine/ObjectCommon/GameObject.h"
#include "Math/Vector/Vector3.h"

/// @brief 壁（Wall）オブジェクト
class WallObject : public GameObject {
public:
    /// @brief 初期化処理
    void Initialize();

    /// @brief 初期化処理（位置・回転・スケール指定）
    /// @param position 位置
    /// @param rotation 回転（Y軸）
    /// @param scale スケール
    void Initialize(const Vector3& position, float rotationY, const Vector3& scale);

    /// @brief 更新処理
    void Update() override;

    /// @brief 描画処理
    /// @param camera カメラ
    void Draw(const ICamera* camera) override;

    RenderPassType GetRenderPassType() const override { return RenderPassType::Model; }

#ifdef _DEBUG
    /// @brief オブジェクト名を取得
    /// @return オブジェクト名
    const char* GetObjectName() const override { return "Wall"; }
#endif

    /// @brief トランスフォームを取得
    WorldTransform& GetTransform() { return transform_; }

    /// @brief モデルを取得
    Model* GetModel() { return model_.get(); }
};
