#include "ResultScene.h"
#include "EngineSystem.h"
#include "Scene/SceneManager.h"
#include "Engine/Graphics/Render/RenderManager.h"
#include "Engine/Graphics/TextureManager.h"

#include <memory>
#include "Application/Utility/KeyBindConfig.h"
#include "Application/Utility/Command/SceneAllCommand.h"
#include "Application/SceneObject/SkyDome/WhiteSkyDome.h"
#include "Application/Utility/MatsumotoUtility.h"

namespace CoreEngine
{
void ResultScene::Initialize(EngineSystem* engine)
{
	BaseScene::Initialize(engine);

	// コンポーネントを直接取得
	auto dxCommon = engine_->GetComponent<DirectXCommon>();
	auto renderManager = engine_->GetComponent<RenderManager>();

	if (!dxCommon || !renderManager) {
		return;
	}

	// リザルトシーンの初期化処理
    sceneCommandExecutor_.Initialize();

    skyDome_ = CreateObject<WhiteSkyDome>();
    skyDome_->SetColor(MatsumotoUtility::ColorYellow);

    // リザルトUIの初期化
    resultUI_ = std::make_unique<ResultUI>(this,&selectedSceneIndex_);
    resultUI_->Initialize();
    selectedSceneIndex_ = 0;

    CoreEngine::SoundManager* soundManager = GetEngineSystem()->GetComponent<CoreEngine::SoundManager>();
    if (!soundManager) {
        assert(false && "SoundManager not found");
    }
    soundResources_.clear();
    soundResources_["Decide"] = soundManager->CreateSoundResource("ApplicationAssets/Sound/Title/SE_Decide.mp3");
    soundResources_["Select"] = soundManager->CreateSoundResource("ApplicationAssets/Sound/SE_Throw.mp3");
    soundResources_["BGM"] = soundManager->CreateSoundResource("ApplicationAssets/Sound/BGM_Result.mp3");
    soundResources_["BGM"]->Play(true);
    soundResources_["BGM"]->SetVolume(0.2f);
}

void ResultScene::OnUpdate()
{
    // 入力処理更新
    KeyBindConfig::Instance().Update();
    // リザルトUIの更新
    resultUI_->Update();

    // 上下キーで選択肢の移動
    KeyBindConfig& keyBindConfig = KeyBindConfig::Instance();
    if (keyBindConfig.GetVerticalAxis() < 0.0) {
        selectedSceneIndex_ = 0;
        if (!soundResources_["Select"]->IsPlaying()) {
            soundResources_["Select"]->Play(false);
        }
        
    } else if (keyBindConfig.GetVerticalAxis() > 0.0) {
        selectedSceneIndex_ = 1;
        if (!soundResources_["Select"]->IsPlaying()) {
            soundResources_["Select"]->Play(false);
        }
    }

    // "Start" キーが押されたらゲームシーンへ遷移
    if (KeyBindConfig::Instance().IsTrigger("Start")) {
        // シーン変更コマンドを追加
        if (selectedSceneIndex_ == 1)
            sceneCommandExecutor_.AddCommand(std::make_unique<SceneChangeCommand>("GameScene", sceneManager_));
        else {
            sceneCommandExecutor_.AddCommand(std::make_unique<SceneChangeCommand>("TitleScene", sceneManager_));
        }
        // 決定音再生
        soundResources_["Decide"]->Play(false);
    }

    // タイトルシーンの更新処理
    sceneCommandExecutor_.ExecuteCommand();
}

void ResultScene::Draw()
{
	BaseScene::Draw();
}

void ResultScene::Finalize()
{
	BaseScene::Finalize();

	// リザルトシーンの解放処理
}
}
