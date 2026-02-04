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

// ポストエフェクト関連
#include "Engine/Graphics/PostEffect/PostEffectManager.h"
#include "Engine/Graphics/PostEffect/PostEffectNames.h"
#include "Engine/Graphics/PostEffect/Effect/Neon.h"
#include "Engine/Graphics/PostEffect/Effect/TransitionEffect.h"
#include "Engine/Graphics/PostEffect/Effect/Scanline.h"
#include "Engine/Utility/FrameRate/FrameRateController.h"

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

	// ポストエフェクトの設定（タイトルシーンと同じ）
	auto postEffectManager = engine_->GetComponent<PostEffectManager>();
	if (postEffectManager) {
		// Neonエフェクトで鮮やかな黄色の光の輪郭を追加
		auto neon = postEffectManager->GetEffect<Neon>(PostEffectNames::Neon);
		if (neon) {
			neon->SetEnabled(true);
			Neon::NeonParams neonParams;
			neonParams.edgeThreshold = 0.25f;
			neonParams.glowIntensity = 3.0f;
			neonParams.edgeWidth = 1.5f;
			neonParams.colorSaturation = 2.0f;
			neonParams.brightness = 1.3f;
			neonParams.neonColorR = 1.0f;
			neonParams.neonColorG = 0.9f;
			neonParams.neonColorB = 0.3f;
			neon->SetParams(neonParams);
		}

		// Scanlineエフェクト
		auto scanline = postEffectManager->GetEffect<CoreEngine::Scanline>(PostEffectNames::Scanline);
		if (scanline) {
			scanline->SetEnabled(true);
			CoreEngine::Scanline::ScanlineParams scanlineParams;
			scanlineParams.lineIntensity = 0.87f;      // 控えめな強度
			scanlineParams.lineWidth = 2.2f;          // 線の幅
			scanlineParams.lineSpeed = 27.0f;         // ゆっくりスクロール
			scanlineParams.lineFrequency = 1.0f;      // 標準的な密度
			scanlineParams.flickerIntensity = 0.03f;  // 微弱なフリッカー
			scanlineParams.flickerSpeed = 8.0f;       // フリッカー速度
			scanline->SetParams(scanlineParams);
		}

		// TransitionEffectの初期化（最初は無効）
		auto transitionEffect = postEffectManager->GetEffect<TransitionEffect>(PostEffectNames::TransitionEffect);
		if (transitionEffect) {
			transitionEffect->SetEnabled(false);
			TransitionEffect::TransitionParams transitionParams;
			transitionParams.progress = 0.0f;
			transitionParams.transitionType = static_cast<int>(TransitionEffect::TransitionType::ZoomBlur);
			transitionParams.centerX = 0.5f;
			transitionParams.centerY = 0.5f;
			transitionParams.smoothness = 0.05f;
			transitionParams.aspectRatio = 1920.0f / 1080.0f;
			transitionParams.maskColorR = 0.0f;
			transitionParams.maskColorG = 0.0f;
			transitionParams.maskColorB = 0.0f;
			transitionParams.slideAngle = 0.0f;
			transitionParams.glitchIntensity = 1.0f;
			transitionParams.pixelSize = 32.0f;
			transitionParams.blindCount = 10.0f;
			transitionParams.randomSeed = 12.345f;
			transitionParams.waveFrequency = 5.0f;
			transitionParams.waveAmplitude = 0.02f;
			transitionEffect->SetParams(transitionParams);
		}
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
    // deltaTimeを取得
    auto frameRateController = engine_->GetComponent<CoreEngine::FrameRateController>();
    float deltaTime = frameRateController ? frameRateController->GetDeltaTime() : (1.0f / 60.0f);

    // Scanlineエフェクトの更新
    {
        auto postEffectManager = engine_->GetComponent<PostEffectManager>();
        if (postEffectManager) {
            auto scanline = postEffectManager->GetEffect<CoreEngine::Scanline>(PostEffectNames::Scanline);
            if (scanline && scanline->IsEnabled()) {
                scanline->Update(deltaTime);
            }
        }
    }

    // トランジション処理
    if (isTransitioning_) {
        auto postEffectMgr = engine_->GetComponent<PostEffectManager>();
        if (postEffectMgr) {
            auto transitionEffect = postEffectMgr->GetEffect<TransitionEffect>(PostEffectNames::TransitionEffect);
            if (transitionEffect) {
                // トランジション進行度を更新
                transitionProgress_ += deltaTime / transitionDuration_;
                if (transitionProgress_ >= 1.0f) {
                    transitionProgress_ = 1.0f;
                }

                transitionEffect->SetProgress(transitionProgress_);

                // トランジションが完了したらシーン遷移
                if (transitionProgress_ >= 1.0f) {
                    sceneCommandExecutor_.AddCommand(std::make_unique<SceneChangeCommand>(nextSceneName_, sceneManager_));
                }
            }
        }
        sceneCommandExecutor_.ExecuteCommand();
        return;  // 遷移中は他の更新処理をスキップ
    }
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

    // "Start" キーが押されたらトランジションを開始
    if (KeyBindConfig::Instance().IsTrigger("Start")) {
        // 決定音再生
        soundResources_["Decide"]->Play(false);

        // トランジションを開始
        auto postEffectMgr = engine_->GetComponent<PostEffectManager>();
        if (postEffectMgr) {
            auto transitionEffect = postEffectMgr->GetEffect<TransitionEffect>(PostEffectNames::TransitionEffect);
            if (transitionEffect) {
                transitionEffect->SetEnabled(true);
                // リザルトシーンからの遷移はズームブラーエフェクト
                transitionEffect->SetTransitionType(TransitionEffect::TransitionType::ZoomBlur);
                transitionProgress_ = 0.0f;
                isTransitioning_ = true;
                
                // 選択されたシーンを設定
                if (selectedSceneIndex_ == 1)
                    nextSceneName_ = "GameScene";
                else
                    nextSceneName_ = "TitleScene";
            }
        }
    }

    // コマンド実行
    sceneCommandExecutor_.ExecuteCommand();
}

void ResultScene::Draw()
{
	BaseScene::Draw();
}

void ResultScene::Finalize()
{
	BaseScene::Finalize();

	// ポストエフェクトを解除
	auto postEffectManager = engine_->GetComponent<PostEffectManager>();
	if (postEffectManager) {
		postEffectManager->SetEffectEnabled(PostEffectNames::Neon, false);
		postEffectManager->SetEffectEnabled(PostEffectNames::Scanline, false);
		postEffectManager->SetEffectEnabled(PostEffectNames::TransitionEffect, false);
	}

	// リザルトシーンの解放処理
}
}
