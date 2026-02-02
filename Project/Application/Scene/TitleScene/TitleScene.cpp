#include "TitleScene.h"
#include "EngineSystem.h"
#include "Scene/SceneManager.h"
#include "Engine/Graphics/Render/RenderManager.h"
#include "Engine/Graphics/TextureManager.h"

#include <memory>
#include <cmath>
#include "Application/Utility/KeyBindConfig.h"
#include "Application/Utility/Command/SceneAllCommand.h"

#include "Application/SceneObject/Player/Player.h"
#include "Application/SceneObject/CosmicTunnel/CosmicTunnelObject.h"
#include "Application/SceneObject/UIFrame/UIFrameObject.h"
#include "Application/Utility/MatsumotoUtility.h"
#include "Engine/Graphics/PostEffect/PostEffectManager.h"
#include "Engine/Graphics/PostEffect/PostEffectNames.h"
#include "Engine/Graphics/PostEffect/Effect/Neon.h"
#include "Engine/Graphics/PostEffect/Effect/TransitionEffect.h"
#include "Engine/Graphics/PostEffect/Effect/Shockwave.h"
#include "Engine/Graphics/PostEffect/Effect/Scanline.h"
#include "Engine/Utility/FrameRate/FrameRateController.h"
#include "Engine/Camera/CameraShake.h"
#include "Engine/Camera/CameraManager.h"
#include "Engine/Camera/Camera2D.h"
#include "Engine/Particle/ParticleSystem.h"
#include "Engine/Graphics/Resource/ResourceFactory.h"
#include "MathCore.h"

namespace CoreEngine
{
    void TitleScene::Initialize(EngineSystem* engine)
    {
        BaseScene::Initialize(engine);

        // コンポーネントを直接取得
        auto dxCommon = engine_->GetComponent<DirectXCommon>();
        auto renderManager = engine_->GetComponent<RenderManager>();
        auto soundManager = engine_->GetComponent<CoreEngine::SoundManager>();
        auto resourceFactory = engine_->GetComponent<ResourceFactory>();
        auto postEffectManager = engine_->GetComponent<PostEffectManager>();

        if (!dxCommon || !renderManager) {
            return;
        }

        // ポストエフェクトの設定（画面を鮮やかに）
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
                transitionParams.transitionType = static_cast<int>(TransitionEffect::TransitionType::ZoomBlur);  // デフォルトは円形
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

        // UI管理クラスの初期化
        uiManager_ = std::make_unique<TitleUIManager>();
        uiManager_->Initialize([this]() {
            return CreateObject<CoreEngine::SpriteObject>();
            });

        // カメラシェイクの初期化
        cameraShake_ = std::make_unique<CoreEngine::CameraShake>();

        // UIManagerにカメラシェイクコールバックを設定
        uiManager_->SetCameraShakeCallback([this](float duration, float intensity) {
            if (cameraShake_) {
                cameraShake_->Start(duration, intensity, 15.0f);  // 周波数15Hzで微弱な振動
            }
            });

        // ショックウェーブコールバック（一時的にコメントアウト）
        
        // UIManagerにショックウェーブコールバックを設定
        //uiManager_->SetShockwaveCallback([this](float centerX, float centerY, float intensity) {
        //    auto postEffectManager = engine_->GetComponent<PostEffectManager>();
        //    if (postEffectManager) {
        //        auto shockwave = postEffectManager->GetEffect<CoreEngine::Shockwave>(PostEffectNames::Shockwave);
        //        if (shockwave) {
        //            // ショックウェーブを開始
        //            shockwave->StartShockwave(centerX, centerY);
        //            shockwave->SetEnabled(true);

        //            // パラメータを設定
        //            CoreEngine::Shockwave::ShockwaveParams params;
        //            params.center[0] = centerX;
        //            params.center[1] = centerY;
        //            params.strength = intensity;  // 小さめの強度
        //            params.thickness = 0.08f;
        //            params.speed = 1.0f;
        //            shockwave->SetParams(params);
        //        }
        //    }
        //    });
        

        // UIManagerにパーティクル発生コールバックを設定
        uiManager_->SetParticleSpawnCallback([this]() {
            if (celebrationParticles_) {
                // 完全にリセット
                celebrationParticles_->Stop();
                celebrationParticles_->Clear();

                // MainModuleとEmissionModuleの両方を完全にリスタート
                auto& mainModule = celebrationParticles_->GetMainModule();
                auto& emissionModule = celebrationParticles_->GetEmissionModule();

                // MainModuleをリスタート（時間をリセット＋再生開始）
                mainModule.Restart();

                // EmissionModuleも完全にリセット
                emissionModule.Stop();
                emissionModule.Play();
            }
            });

        // 浮遊パーティクルシステムの初期化
        if (resourceFactory) {
            auto particleSystem = CreateObject<ParticleSystem>();
            particleSystem->Initialize(dxCommon, resourceFactory, "TitleFloatingParticles");

            // 基本設定
            particleSystem->SetEmitterPosition({ 0.0f, -5.0f, 10.0f });
            particleSystem->SetBlendMode(BlendMode::kBlendModeAdd);
            particleSystem->SetBillboardType(BillboardType::ViewFacing);
            particleSystem->SetTexture("Particle/circle.png");

            // MainModule設定（より多くのパーティクル、長いライフタイム）
            auto& mainModule = particleSystem->GetMainModule();
            mainModule.GetMainData().duration = 0.0f;
            mainModule.GetMainData().looping = true;
            mainModule.GetMainData().maxParticles = 800;
            mainModule.GetMainData().startLifetime = 15.0f;
            mainModule.GetMainData().startLifetimeRandomness = 0.5f;
            mainModule.GetMainData().startSpeed = 0.8f;
            mainModule.GetMainData().startSpeedRandomness = 0.7f;
            mainModule.GetMainData().startSize = { 0.12f, 0.12f, 0.12f };
            mainModule.GetMainData().startSizeRandomness = 0.8f;
            mainModule.GetMainData().startRotation = { 0.0f, 0.0f, 0.0f };
            mainModule.GetMainData().startRotationRandomness = 1.0f;
            mainModule.GetMainData().startColor = { 0.4f, 0.7f, 1.0f, 0.6f };
            mainModule.GetMainData().startColorRandomness = 0.4f;
            mainModule.GetMainData().gravityModifier = -0.05f;

            // EmissionModule設定（より多くのパーティクルを発生）
            auto& emissionModule = particleSystem->GetEmissionModule();
            emissionModule.SetEnabled(true);
            EmissionModule::EmissionData emissionData;
            emissionData.rateOverTime = 25;
            emissionModule.SetEmissionData(emissionData);

            // ShapeModule設定（より広範囲から発生）
            auto& shapeModule = particleSystem->GetShapeModule();
            shapeModule.SetEnabled(true);
            shapeModule.GetShapeData().shapeType = ShapeModule::ShapeType::Box;
            shapeModule.GetShapeData().scale = { 50.0f, 30.0f, 15.0f };

            // VelocityModule設定（ランダムな方向に飛ぶ）
            auto& velocityModule = particleSystem->GetVelocityModule();
            velocityModule.SetEnabled(true);
            VelocityModule::VelocityData velocityData;
            velocityData.startSpeed = { 0.0f, 1.0f, 0.0f };
            velocityData.randomSpeedRange = { 1.0f, 1.0f, 1.0f };
            velocityData.useRandomDirection = true;
            velocityModule.SetVelocityData(velocityData);

            // ForceModule設定（風と渦巻く動き）
            auto& forceModule = particleSystem->GetForceModule();
            forceModule.SetEnabled(true);
            ForceModule::ForceData forceData;
            forceData.gravity = { 0.0f, 0.1f, 0.0f };
            forceData.wind = { 0.3f, 0.2f, 0.15f };
            forceData.drag = 0.1f;
            forceModule.SetForceData(forceData);

            // ColorModule設定（より鮮やかな色のグラデーション）
            auto& colorModule = particleSystem->GetColorModule();
            colorModule.SetEnabled(true);
            ColorModule::ColorOverLifetime colorData;
            colorData.endColor = { 0.2f, 0.5f, 1.0f, 0.0f };
            colorData.useGradient = true;
            colorModule.SetColorData(colorData);

            // SizeModule設定（動的なサイズ変化）
            auto& sizeModule = particleSystem->GetSizeModule();
            sizeModule.SetEnabled(true);
            SizeModule::SizeData sizeData;
            sizeData.endSize = 0.02f;
            sizeData.sizeOverLifetime = true;
            sizeData.sizeCurve = SizeModule::SizeData::SizeCurve::EaseOut;
            sizeModule.SetSizeData(sizeData);

            // RotationModule設定（より速い回転）
            auto& rotationModule = particleSystem->GetRotationModule();
            rotationModule.SetEnabled(true);
            RotationModule::RotationData rotationData;
            rotationData.use2DRotation = true;
            rotationData.rotation2DSpeed = 1.2f;
            rotationData.rotation2DSpeedRandomness = 1.5f;
            rotationData.rotationDirection = RotationModule::RotationData::RotationDirection::Both;
            rotationModule.SetRotationData(rotationData);

            // NoiseModule設定（より強い揺らぎ）
            auto& noiseModule = particleSystem->GetNoiseModule();
            noiseModule.SetEnabled(true);
            NoiseModule::NoiseData noiseData;
            noiseData.strength = 1.5f;
            noiseData.frequency = 0.8f;
            noiseData.scrollSpeed = 0.4f;
            noiseData.damping = true;
            noiseData.positionAmount = { 1.2f, 1.0f, 1.2f };
            noiseModule.SetNoiseData(noiseData);

            floatingParticles_ = particleSystem;
            floatingParticles_->Play();
        }

        // キラキラパーティクルシステムの初期化（小さく輝く星のような演出）
        if (resourceFactory) {
            auto sparkleSystem = CreateObject<ParticleSystem>();
            sparkleSystem->Initialize(dxCommon, resourceFactory, "TitleSparkles");

            sparkleSystem->SetEmitterPosition({ 0.0f, 0.0f, 8.0f });
            sparkleSystem->SetBlendMode(BlendMode::kBlendModeAdd);
            sparkleSystem->SetBillboardType(BillboardType::ViewFacing);
            sparkleSystem->SetTexture("Particle/star.png");

            // MainModule設定
            auto& mainModule = sparkleSystem->GetMainModule();
            mainModule.GetMainData().duration = 0.0f;
            mainModule.GetMainData().looping = true;
            mainModule.GetMainData().maxParticles = 150;
            mainModule.GetMainData().startLifetime = 3.0f;
            mainModule.GetMainData().startLifetimeRandomness = 0.6f;
            mainModule.GetMainData().startSpeed = 0.1f;
            mainModule.GetMainData().startSpeedRandomness = 0.8f;
            mainModule.GetMainData().startSize = { 0.06f, 0.06f, 0.06f };
            mainModule.GetMainData().startSizeRandomness = 0.9f;
            mainModule.GetMainData().startRotation = { 0.0f, 0.0f, 0.0f };
            mainModule.GetMainData().startRotationRandomness = 1.0f;
            mainModule.GetMainData().startColor = { 1.0f, 0.9f, 0.6f, 0.8f };
            mainModule.GetMainData().startColorRandomness = 0.3f;
            mainModule.GetMainData().gravityModifier = 0.0f;

            // EmissionModule設定
            auto& emissionModule = sparkleSystem->GetEmissionModule();
            emissionModule.SetEnabled(true);
            EmissionModule::EmissionData emissionData;
            emissionData.rateOverTime = 15;
            emissionModule.SetEmissionData(emissionData);

            // ShapeModule設定
            auto& shapeModule = sparkleSystem->GetShapeModule();
            shapeModule.SetEnabled(true);
            shapeModule.GetShapeData().shapeType = ShapeModule::ShapeType::Box;
            shapeModule.GetShapeData().scale = { 55.0f, 35.0f, 20.0f };

            // VelocityModule設定
            auto& velocityModule = sparkleSystem->GetVelocityModule();
            velocityModule.SetEnabled(true);
            VelocityModule::VelocityData velocityData;
            velocityData.startSpeed = { 0.0f, 0.0f, 0.0f };
            velocityData.randomSpeedRange = { 0.3f, 0.3f, 0.3f };
            velocityData.useRandomDirection = true;
            velocityModule.SetVelocityData(velocityData);

            // ColorModule設定（点滅するような演出）
            auto& colorModule = sparkleSystem->GetColorModule();
            colorModule.SetEnabled(true);
            ColorModule::ColorOverLifetime colorData;
            colorData.endColor = { 1.0f, 1.0f, 1.0f, 0.0f };
            colorData.useGradient = true;
            colorModule.SetColorData(colorData);

            // SizeModule設定（パルス的に大きくなって消える）
            auto& sizeModule = sparkleSystem->GetSizeModule();
            sizeModule.SetEnabled(true);
            SizeModule::SizeData sizeData;
            sizeData.endSize = 0.01f;
            sizeData.sizeOverLifetime = true;
            sizeData.sizeCurve = SizeModule::SizeData::SizeCurve::EaseOut;
            sizeModule.SetSizeData(sizeData);

            // RotationModule設定
            auto& rotationModule = sparkleSystem->GetRotationModule();
            rotationModule.SetEnabled(true);
            RotationModule::RotationData rotationData;
            rotationData.use2DRotation = true;
            rotationData.rotation2DSpeed = 2.0f;
            rotationData.rotation2DSpeedRandomness = 2.0f;
            rotationData.rotationDirection = RotationModule::RotationData::RotationDirection::Both;
            rotationModule.SetRotationData(rotationData);

            sparkleParticles_ = sparkleSystem;
            sparkleParticles_->Play();
        }

        // お祝いパーティクルシステムの初期化（指が回転させたときに発生）
        if (resourceFactory) {
            auto celebrationSystem = CreateObject<ParticleSystem>();
            celebrationSystem->Initialize(dxCommon, resourceFactory, "TitleCelebration");

            // 画面上部中央から発生
            celebrationSystem->SetEmitterPosition({ 0.0f, 15.0f, 8.0f });
            celebrationSystem->SetBlendMode(BlendMode::kBlendModeAdd);
            celebrationSystem->SetBillboardType(BillboardType::ViewFacing);
            celebrationSystem->SetTexture("Particle/star.png");

            // MainModule設定
            auto& mainModule = celebrationSystem->GetMainModule();
            mainModule.GetMainData().duration = 1.5f;  // 1.5秒間発生
            mainModule.GetMainData().looping = false;  // 1回のみ
            mainModule.GetMainData().maxParticles = 100;
            mainModule.GetMainData().startLifetime = 2.5f;
            mainModule.GetMainData().startLifetimeRandomness = 0.5f;
            mainModule.GetMainData().startSpeed = 8.0f;
            mainModule.GetMainData().startSpeedRandomness = 0.6f;
            mainModule.GetMainData().startSize = { 0.15f, 0.15f, 0.15f };
            mainModule.GetMainData().startSizeRandomness = 0.7f;
            mainModule.GetMainData().startRotation = { 0.0f, 0.0f, 0.0f };
            mainModule.GetMainData().startRotationRandomness = 1.0f;
            mainModule.GetMainData().startColor = { 1.0f, 0.8f, 0.2f, 1.0f };  // 金色
            mainModule.GetMainData().startColorRandomness = 0.3f;
            mainModule.GetMainData().gravityModifier = 2.0f;  // 重力で下に落ちる

            // EmissionModule設定（バースト発生）
            auto& emissionModule = celebrationSystem->GetEmissionModule();
            emissionModule.SetEnabled(true);
            EmissionModule::EmissionData emissionData;
            emissionData.rateOverTime = 0;  // 継続発生なし
            emissionData.burstCount = 80;   // 一度に80個発生
            emissionData.burstTime = 0.0f;  // すぐに発生
            emissionModule.SetEmissionData(emissionData);

            // ShapeModule設定（画面上部全体から発生）
            auto& shapeModule = celebrationSystem->GetShapeModule();
            shapeModule.SetEnabled(true);
            shapeModule.GetShapeData().shapeType = ShapeModule::ShapeType::Box;
            shapeModule.GetShapeData().scale = { 60.0f, 2.0f, 5.0f };  // 横幅広く、縦は薄く

            // VelocityModule設定（下向きに落下）
            auto& velocityModule = celebrationSystem->GetVelocityModule();
            velocityModule.SetEnabled(true);
            VelocityModule::VelocityData velocityData;
            velocityData.startSpeed = { 0.0f, -5.0f, 0.0f };  // 下向きに落下
            velocityData.randomSpeedRange = { 2.0f, 3.0f, 0.5f };  // X方向に少し散らばる
            velocityData.useRandomDirection = false;
            velocityModule.SetVelocityData(velocityData);

            // ColorModule設定（徐々にフェードアウト）
            auto& colorModule = celebrationSystem->GetColorModule();
            colorModule.SetEnabled(true);
            ColorModule::ColorOverLifetime colorData;
            colorData.endColor = { 1.0f, 0.5f, 0.0f, 0.0f };  // オレンジ色にフェード
            colorData.useGradient = true;
            colorModule.SetColorData(colorData);

            // SizeModule設定（徐々に小さくなる）
            auto& sizeModule = celebrationSystem->GetSizeModule();
            sizeModule.SetEnabled(true);
            SizeModule::SizeData sizeData;
            sizeData.endSize = 0.02f;
            sizeData.sizeOverLifetime = true;
            sizeData.sizeCurve = SizeModule::SizeData::SizeCurve::EaseOut;
            sizeModule.SetSizeData(sizeData);

            // RotationModule設定（回転）
            auto& rotationModule = celebrationSystem->GetRotationModule();
            rotationModule.SetEnabled(true);
            RotationModule::RotationData rotationData;
            rotationData.use2DRotation = true;
            rotationData.rotation2DSpeed = 3.0f;
            rotationData.rotation2DSpeedRandomness = 2.0f;
            rotationData.rotationDirection = RotationModule::RotationData::RotationDirection::Both;
            rotationModule.SetRotationData(rotationData);

            celebrationParticles_ = celebrationSystem;
            // 最初は再生しない（コールバックで再生）
        }

        // サウンドの読み込みと再生
        if (soundManager) {
            // BGMの読み込みと再生（ループ）
            bgm_ = soundManager->CreateSoundResource("Assets/ApplicationAssets/Sound/Title/BGM_Title.mp3");
            if (bgm_ && bgm_->IsValid()) {
                bgm_->SetVolume(0.5f);  // 音量を50%に設定
                bgm_->Play(true);  // ループ再生
            }

            // SE（決定音）の読み込み
            decideSE_ = soundManager->CreateSoundResource("Assets/ApplicationAssets/Sound/Title/SE_Decide.mp3");
            if (decideSE_ && decideSE_->IsValid()) {
                decideSE_->SetVolume(0.7f);  // 音量を70%に設定
            }

            // タイトルロゴ回転SEの読み込み
            auto rotateSE = soundManager->CreateSoundResource("Assets/ApplicationAssets/Sound/Title/mawasuSe.mp3");
            if (rotateSE && rotateSE->IsValid()) {
                rotateSE->SetVolume(0.6f);  // 音量を60%に設定
            }

            // タイトルロゴ元に戻るSEの読み込み
            auto returnSE = soundManager->CreateSoundResource("Assets/ApplicationAssets/Sound/Title/modoruSe.mp3");
            if (returnSE && returnSE->IsValid()) {
                returnSE->SetVolume(1.0f);  // 音量を60%に設定
            }

            // UIManagerにサウンドリソースを設定
            if (uiManager_) {
                uiManager_->SetSoundResources(std::move(rotateSE), std::move(returnSE));
            }
        }

    }


    void TitleScene::OnUpdate()
    {
        // 入力処理更新
        KeyBindConfig::Instance().Update();

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

        // UI更新
        if (uiManager_) {
            uiManager_->Update(deltaTime);
        }


        // カメラシェイクの更新（2Dカメラをシェイク）
        if (cameraShake_ && cameraManager_) {
            cameraShake_->Update(deltaTime);

            // 2Dカメラを取得
            auto activeCamera2D = cameraManager_->GetActiveCamera(CameraType::Camera2D);
            if (activeCamera2D) {
                auto camera2D = dynamic_cast<CoreEngine::Camera2D*>(activeCamera2D);
                if (camera2D) {
                    if (cameraShake_->IsActive()) {
                        // シェイク開始時に元の位置を保存
                        if (!isShaking_) {
                            originalCamera2DPosition_ = camera2D->GetPosition();
                            isShaking_ = true;
                        }

                        // 元の位置にシェイクオフセットを加算
                        Vector3 shakeOffset = cameraShake_->GetOffset();
                        camera2D->SetPosition(originalCamera2DPosition_ + shakeOffset);
                    } else {
                        // シェイク終了時に元の位置に戻す
                        if (isShaking_) {
                            camera2D->SetPosition(originalCamera2DPosition_);
                            isShaking_ = false;
                        }
                    }
                }
            }
        }

        // ショックウェーブの更新（一時的にコメントアウト）
        
       /* {
            auto postEffectMgr = engine_->GetComponent<PostEffectManager>();
            if (postEffectMgr) {
                auto shockwave = postEffectMgr->GetEffect<CoreEngine::Shockwave>(PostEffectNames::Shockwave);
                if (shockwave && shockwave->IsActive()) {
                    shockwave->Update(deltaTime);
                }
            }
        }*/
        

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
                        sceneCommandExecutor_.AddCommand(std::make_unique<SceneChangeCommand>("GameScene", sceneManager_));
                    }
                }
            }
        } else {
            // 退出アニメーション完了後にトランジション開始
            if (uiManager_ && uiManager_->IsExitAnimationComplete()) {
                auto postEffectMgr = engine_->GetComponent<PostEffectManager>();
                if (postEffectMgr) {
                    auto transitionEffect = postEffectMgr->GetEffect<TransitionEffect>(PostEffectNames::TransitionEffect);
                    if (transitionEffect) {
                        transitionEffect->SetEnabled(true);
                        transitionProgress_ = 0.0f;
                        isTransitioning_ = true;
                    }
                }
            }
        }

        // "Start" キーが押されたら退出アニメーション開始
        if (KeyBindConfig::Instance().IsTrigger("Start")) {
            if (uiManager_) {
                uiManager_->StartExitAnimation();
            }

            //seを再生
            decideSE_->Play();
        }

        // コマンド実行
        sceneCommandExecutor_.ExecuteCommand();
    }


    void TitleScene::Draw()
    {
        BaseScene::Draw();
    }

    void TitleScene::Finalize()
    {
        // BGMを停止
        if (bgm_ && bgm_->IsValid()) {
            bgm_->Stop();
        }

        //ポストエフェクトを解除
        auto postEffectManager = engine_->GetComponent<PostEffectManager>();
        if (postEffectManager)
        {
            postEffectManager->SetEffectEnabled(PostEffectNames::Neon, false);
            postEffectManager->SetEffectEnabled(PostEffectNames::Scanline, false);
            postEffectManager->SetEffectEnabled(PostEffectNames::TransitionEffect, false);
        }

        BaseScene::Finalize();
    }
}
