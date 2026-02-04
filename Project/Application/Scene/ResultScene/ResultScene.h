#pragma once
#include "Scene/BaseScene.h"
#include "EngineSystem/EngineSystem.h"

#include "Application/Utility/Command/SceneCommandExecutor.h"

#include <memory>
#include "Application/SceneObject/ScreenUI/ResultUI.h"

class WhiteSkyDome;
namespace CoreEngine
{
/// @brief リザルトシーンクラス
class ResultScene : public BaseScene {
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
    WhiteSkyDome* skyDome_;

    std::unique_ptr<ResultUI> resultUI_;
    int selectedSceneIndex_;
    std::map<std::string, std::unique_ptr<CoreEngine::SoundManager::SoundResource>> soundResources_;

    // シーン遷移用
    float transitionProgress_ = 0.0f;
    float transitionDuration_ = 1.0f;  // トランジション時間（秒）
    bool isTransitioning_ = false;
    std::string nextSceneName_ = "";
};
}
