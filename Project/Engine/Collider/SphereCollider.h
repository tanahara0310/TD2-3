#pragma once
#include "Collider.h"

// 前方宣言
namespace CoreEngine {
    class LineRendererPipeline;
    class ICamera;
}

namespace CoreEngine
{
class SphereCollider : public Collider {
public:
   SphereCollider(GameObject* owner, float r);

   bool CheckCollision(Collider* other) const override;

   void SetRadius(float radius) override;

   /// @brief 球の半径を取得
   float GetRadius() const { return radius_; }

#ifdef _DEBUG
   /// @brief デバッグ用にコライダーを描画
   /// @param pipeline LineRendererPipeline
   /// @param camera カメラ
   /// @param color ラインの色（デフォルト: 緑）
   void DrawDebug(CoreEngine::LineRendererPipeline* pipeline, const CoreEngine::ICamera* camera, 
                  const Vector3& color = {0.0f, 1.0f, 0.0f}) const;
#endif

private:
   float radius_{};
};
}
