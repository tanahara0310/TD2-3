#include "TitleScene.h"
#include "EngineSystem.h"
#include "Scene/SceneManager.h"
#include "Engine/Graphics/Render/RenderManager.h"
#include "Engine/Graphics/TextureManager.h"

#include <memory>
#include "Application/Utility/KeyBindConfig.h"
#include "Application/Utility/Command/SceneAllCommand.h"

#include "Application/SceneObject/Player/Player.h"
#include "Application/SceneObject/Ball/Ball.h"
#include "Application/SceneObject/SkyDome/WhiteSkyDome.h"
#include "Application/Utility/MatsumotoUtility.h"

namespace CoreEngine
{
void TitleScene::Initialize(EngineSystem* engine)
{
	BaseScene::Initialize(engine);

	// コンポーネントを直接取得
	auto dxCommon = engine_->GetComponent<DirectXCommon>();
	auto renderManager = engine_->GetComponent<RenderManager>();

	if (!dxCommon || !renderManager) {
		return;
	}

	// タイトルシーンの初期化処理
    sceneCommandExecutor_.Initialize();

    player_ = CreateObject<Player>();
    ball_ = CreateObject<Ball>();
    player_->Initialize();
    ball_->Initialize();
    ballController_ = std::make_unique<BallController>(ball_, player_, this);

    skyDome_ = CreateObject<WhiteSkyDome>();
    skyDome_->SetColor(MatsumotoUtility::ColorEggplant);

    // タイトル画像の作成
    titleSprite_ = CreateObject<CoreEngine::SpriteObject>();
    titleSprite_->Initialize("Assets/Texture/Title/title.png", "TitleLogo");
    titleSprite_->GetSpriteTransform().translate = { 0.0f, 200.0f, 0.0f };
    titleSprite_->SetAnchor({ 0.5f, 0.5f });
    titleSprite_->GetSpriteTransform().scale = { 1.0f, 1.0f, 1.0f };

    // スペーススタート画像の作成
    spaceStartSprite_ = CreateObject<CoreEngine::SpriteObject>();
    spaceStartSprite_->Initialize("Assets/Texture/Title/spaceStart.png", "SpaceStart");
    spaceStartSprite_->GetSpriteTransform().translate = { 0.0f, -200.0f, 0.0f };
    spaceStartSprite_->SetAnchor({ 0.5f, 0.5f });
    spaceStartSprite_->GetSpriteTransform().scale = { 1.0f, 1.0f, 1.0f };

    // ヨーヨーオブジェクトを作成
    yoyo_ = CreateObject<CoreEngine::YoYoObject>();
    yoyo_->Initialize();
    
    // 演出用の初期設定
    yoyoState_ = YoYoAnimationState::Descending;
    yoyoAnimationTime_ = 0.0f;
    yoyoRotationSpeed_ = 1.0f;
    yoyoTargetPosition_ = { 0.0f, 0.0f, 5.0f };
    
    // 初期位置（画面左上奥）
    yoyo_->SetPosition({ -8.0f, 6.0f, 12.0f });
    yoyo_->SetScale({ 3.0f, 3.0f, 3.0f });

}

void TitleScene::OnUpdate()
{
    // 入力処理更新
    KeyBindConfig::Instance().Update();

    // "Start" キーが押されたらゲームシーンへ遷移
    if (KeyBindConfig::Instance().IsTrigger("Start")) {
        // シーン変更コマンドを追加
        sceneCommandExecutor_.AddCommand(std::make_unique<SceneChangeCommand>("GameScene", sceneManager_));
    }

    player_->Update();
    ball_->Update();
    ballController_->Update();

    // ヨーヨー演出の更新
    UpdateYoYoAnimation();

    // タイトルシーンの更新処理
    sceneCommandExecutor_.ExecuteCommand();
}

void TitleScene::UpdateYoYoAnimation()
{
    yoyoAnimationTime_ += 0.016f; // 約60FPS想定

    switch (yoyoState_)
    {
    case YoYoAnimationState::Descending:
    {
        // 大きな8の字軌道で画面を駆け巡る（4秒間）
        float t = yoyoAnimationTime_ / 4.0f;
        if (t >= 1.0f) {
            t = 1.0f;
            yoyoState_ = YoYoAnimationState::Spinning;
            yoyoAnimationTime_ = 0.0f;
        }

        // リサージュ曲線（8の字）
        float angle = t * 3.14159f * 4.0f; // 2周
        
        // X軸: 大きく左右に振る（-8 ~ 8）
        float x = sin(angle) * 8.0f;
        
        // Y軸: 上下に大きく動く（-3 ~ 6）
        float y = sin(angle * 2.0f) * 4.5f + 1.5f;
        
        // Z軸: 手前と奥を行き来（3 ~ 12）
        float z = 7.5f + cos(angle) * 4.5f;
        
        Vector3 currentPos = { x, y, z };
        yoyo_->SetPosition(currentPos);

        // 回転速度を急加速
        yoyoRotationSpeed_ = 1.0f + t * 15.0f; // 1.0 -> 16.0
        yoyo_->SetRotationSpeed(yoyoRotationSpeed_);
        
        // スケールを変化（遠近感）
        float scale = 2.5f + cos(angle) * 1.0f; // 1.5 ~ 3.5
        yoyo_->SetScale({ scale, scale, scale });
        
        // 徐々に火花を出し始める
        yoyo_->SetSparkIntensity(t * 0.5f);
        break;
    }

    case YoYoAnimationState::Spinning:
    {
        // 画面中央で円運動しながら高速スピン（3秒間）
        if (yoyoAnimationTime_ >= 3.0f) {
            yoyoState_ = YoYoAnimationState::Bursting;
            yoyoAnimationTime_ = 0.0f;
        }

        // 円運動（アラウンド・ザ・ワールド）
        float circleAngle = yoyoAnimationTime_ * 3.0f; // 高速回転
        float circleRadius = 5.0f;
        
        float x = sin(circleAngle) * circleRadius;
        float y = cos(circleAngle) * circleRadius;
        float z = 5.0f + sin(circleAngle * 2.0f) * 2.0f; // Z軸も変化
        
        yoyo_->SetPosition({ x, y, z });

        // 超高速回転
        yoyoRotationSpeed_ = 15.0f + sin(yoyoAnimationTime_ * 5.0f) * 5.0f; // 10.0 ~ 20.0
        yoyo_->SetRotationSpeed(yoyoRotationSpeed_);

        // 火花を激しく
        float sparkPulse = sin(yoyoAnimationTime_ * 10.0f) * 0.5f + 1.0f;
        yoyo_->SetSparkIntensity(sparkPulse * 1.5f);
        
        // スケールをパルス
        float scale = 2.5f + sin(yoyoAnimationTime_ * 8.0f) * 0.5f;
        yoyo_->SetScale({ scale, scale, scale });
        break;
    }

    case YoYoAnimationState::Bursting:
    {
        // 画面中央で火花爆発（3秒間）
        if (yoyoAnimationTime_ >= 3.0f) {
            yoyoState_ = YoYoAnimationState::Idle;
            yoyoAnimationTime_ = 0.0f;
        }

        float t = yoyoAnimationTime_ / 3.0f;
        
        // 中央に向かってゆっくり移動
        Vector3 centerPos = { 0.0f, 0.0f, 5.0f };
        Vector3 startPos = yoyo_->GetPosition();
        Vector3 currentPos;
        currentPos.x = startPos.x + (centerPos.x - startPos.x) * t;
        currentPos.y = startPos.y + (centerPos.y - startPos.y) * t;
        currentPos.z = startPos.z + (centerPos.z - startPos.z) * t;
        yoyo_->SetPosition(currentPos);

        // 回転速度を徐々に落とす
        yoyoRotationSpeed_ = 20.0f * (1.0f - t) + 3.0f * t; // 20.0 -> 3.0
        yoyo_->SetRotationSpeed(yoyoRotationSpeed_);

        // 火花を超激しく（爆発）
        float sparkBurst = (1.0f - t * 0.5f) * (sin(yoyoAnimationTime_ * 25.0f) * 0.5f + 2.0f);
        yoyo_->SetSparkIntensity(sparkBurst * 3.0f); // 最大9倍
        
        // パルス効果（大きく）
        float pulse = 1.0f + sin(yoyoAnimationTime_ * 20.0f) * 0.3f;
        yoyo_->SetScale({ 3.0f * pulse, 3.0f * pulse, 3.0f * pulse });
        break;
    }

    case YoYoAnimationState::Idle:
    {
        // ゆっくり回転しながら待機
        yoyoRotationSpeed_ = 3.0f;
        yoyo_->SetRotationSpeed(yoyoRotationSpeed_);
        yoyo_->SetScale({ 3.0f, 3.0f, 3.0f });
        
        // 火花を徐々に消す
        float fadeOut = 1.0f - (yoyoAnimationTime_ / 2.0f);
        if (fadeOut < 0.0f) fadeOut = 0.0f;
        yoyo_->SetSparkIntensity(fadeOut * 0.5f);

        // 2秒後にループ
        if (yoyoAnimationTime_ >= 2.0f) {
            yoyoState_ = YoYoAnimationState::Descending;
            yoyoAnimationTime_ = 0.0f;
            yoyo_->SetPosition({ -8.0f, 6.0f, 12.0f });
        }
        break;
    }
    }
}

void TitleScene::Draw()
{
	BaseScene::Draw();

	// タイトルシーンの描画処理
}

void TitleScene::Finalize()
{
	BaseScene::Finalize();

	// タイトルシーンの解放処理
}
}
