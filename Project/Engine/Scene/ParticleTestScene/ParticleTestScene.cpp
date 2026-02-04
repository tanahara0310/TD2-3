#include "ParticleTestScene.h"
#include "Engine/Graphics/Common/DirectXCommon.h"
#include "Engine/Graphics/Resource/ResourceFactory.h"

#ifdef _DEBUG
#include "Engine/Camera/Debug/CameraDebugUI.h"
#endif

namespace CoreEngine
{
    void ParticleTestScene::Initialize(EngineSystem* engine)
    {
        // 基底クラスの初期化（カメラ、ライト、グリッドのセットアップ）
        BaseScene::Initialize(engine);

        // コンポーネントを取得
        auto dxCommon = engine_->GetComponent<DirectXCommon>();
        auto resourceFactory = engine_->GetComponent<ResourceFactory>();

        if (!dxCommon || !resourceFactory) {
            return; // 必須コンポーネントがない場合は終了
        }

        // ===== テスト用パーティクルシステムの初期化 =====
        auto particleSystem = CreateObject<ParticleSystem>();
        particleSystem->Initialize(dxCommon, resourceFactory, "TestParticle");

        // パーティクルシステムの基本設定
        particleSystem->SetEmitterPosition({ 0.0f, 0.0f, 0.0f });
        particleSystem->SetBlendMode(BlendMode::kBlendModeAdd);
        particleSystem->SetBillboardType(BillboardType::ViewFacing);

        particleSystem_ = particleSystem;

        // パーティクルを再生開始
        particleSystem_->Play();

        // ===== 敵死亡時パーティクルシステムの初期化（プリセット読み込み） =====
        auto enemyDeathParticle = CreateObject<ParticleSystem>();
        enemyDeathParticle->Initialize(dxCommon, resourceFactory, "EnemyDeathParticle");

        // プリセットマネージャーを使用してEnemyDeath.jsonを読み込み
        enemyDeathParticle->GetPresetManager().LoadPreset(enemyDeathParticle, "Assets/Presets/Particle/EnemyDeath.json");

        // エミッター位置を調整（右側に配置）
        enemyDeathParticle->SetEmitterPosition({ 3.0f, 2.0f, 0.0f });

        enemyDeathParticle_ = enemyDeathParticle;

        // 敵死亡パーティクルを再生開始
        enemyDeathParticle_->Play();

        // ===== 敵リスポーン時パーティクルシステムの初期化（パーティクルが中心に集まる） =====
        auto enemyRespawnParticle = CreateObject<ParticleSystem>();
        enemyRespawnParticle->Initialize(dxCommon, resourceFactory, "EnemyRespawnParticle");

        // プリセットマネージャーを使用してEnemyRespawn.jsonを読み込み
        enemyRespawnParticle->GetPresetManager().LoadPreset(enemyRespawnParticle, "Assets/Presets/Particle/EnemyRespawn.json");

        // エミッター位置を調整（左側に配置）
        enemyRespawnParticle->SetEmitterPosition({ -3.0f, 2.0f, 0.0f });

        // アトラクターポイントも同じ位置に設定（中心に向かう）
        enemyRespawnParticle->GetForceModule().GetForceData().attractorPoint = { -3.0f, 2.0f, 0.0f };

        enemyRespawnParticle_ = enemyRespawnParticle;

        // リスポーンパーティクルを再生開始
        enemyRespawnParticle_->Play();

        // タイマーを初期化（リスポーンパーティクルを2.5秒後に開始）
        deathParticleTimer_ = 0.0f;
        respawnParticleTimer_ = kParticleInterval_ - 2.5f;  // 2.5秒後にスタート

        // ===== 数字表示の初期化 =====
        numberDisplay_ = std::make_unique<NumberDisplayUtility>();
        // CreateObjectを渡してスプライトを生成
        numberDisplay_->Initialize([this]() {
            return CreateObject<SpriteObject>();
            }, "Texture/Number/", 8);

        // 初期スコアを設定
        testScore_ = 12345;

        // 画面上部中央にスコアを表示
        numberDisplay_->DisplayNumberCentered(testScore_, { 640.0f, 100.0f }, { 1.5f, 1.5f }, 10.0f);
        numberDisplay_->SetColor({ 1.0f, 1.0f, 0.0f, 1.0f }); // 黄色


    }

    void ParticleTestScene::OnUpdate()
    {
        // パーティクルテストシーン固有の更新処理

        // タイマーを更新
        deathParticleTimer_ += 1.0f / 60.0f;  // 60FPS想定
        respawnParticleTimer_ += 1.0f / 60.0f;

        // 敵死亡パーティクル：一定間隔で再生
        if (deathParticleTimer_ >= kParticleInterval_) {
            if (enemyDeathParticle_ && enemyDeathParticle_->IsFinished()) {
                enemyDeathParticle_->Clear();
                enemyDeathParticle_->Play();
                deathParticleTimer_ = 0.0f;
            }
        }

        // 敵リスポーンパーティクル：一定間隔で再生（死亡パーティクルとずらす）
        if (respawnParticleTimer_ >= kParticleInterval_) {
            if (enemyRespawnParticle_ && enemyRespawnParticle_->IsFinished()) {
                enemyRespawnParticle_->Clear();
                enemyRespawnParticle_->Play();
                respawnParticleTimer_ = 0.0f;
            }
        }

        // 数字表示の更新
        if (numberDisplay_) {
            numberDisplay_->Update();
        }
    }

    void ParticleTestScene::Draw()
    {
        // 基底クラスの描画（全てのゲームオブジェクトとパーティクルの描画）
        BaseScene::Draw();
    }

    void ParticleTestScene::Finalize()
    {
        // 基底クラスの解放
        BaseScene::Finalize();
    }
}
