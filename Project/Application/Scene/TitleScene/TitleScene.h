#pragma once
#include "Scene/BaseScene.h"
#include "EngineSystem/EngineSystem.h"

#include "Application/Utility/Command/SceneCommandExecutor.h"

class Player;
class Ball;
class WhiteSkyDome;
#include "Application/SceneObject/Menu/MenuView.h"
#include "Application/SceneObject/Ball/BallController.h"

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

    Player* player_;
    Ball* ball_;
    WhiteSkyDome* skyDome_;
    std::unique_ptr<BallController> ballController_;
};
}
