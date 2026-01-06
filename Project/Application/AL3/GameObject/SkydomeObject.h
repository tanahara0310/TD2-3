#pragma once

#include "Engine/ObjectCommon/GameObject.h"

/// @brief 天球（Skydome）オブジェクト
class SkydomeObject : public GameObject {
public:
    /// @brief 初期化処理
    void Initialize();

    /// @brief 更新処理
    void Update() override;

    /// @brief 描画処理
    /// @param camera カメラ
    void Draw(const ICamera* camera) override;

	RenderPassType GetRenderPassType() const override { return RenderPassType::Model; }

#ifdef _DEBUG
    /// @brief オブジェクト名を取得
    /// @return オブジェクト名
    const char* GetObjectName() const override { return "Skydome"; }
#endif

    /// @brief トランスフォームを取得
    WorldTransform& GetTransform() { return transform_; }

    /// @brief モデルを取得
    Model* GetModel() { return model_.get(); }
};
