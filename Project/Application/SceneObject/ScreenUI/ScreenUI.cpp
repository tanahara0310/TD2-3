#include "ScreenUI.h"
#include "Application//Utility/MatsumotoUtility.h"
#include "Application/Utility/KeyBindConfig.h"
#include "Application/SceneObject/Player/Player.h"
#include "Application/Utility/Stopwatch.h"
#include "Application/SceneObject/Menu/MenuController.h"
#include "Application/SceneObject/Ball/BallController.h"

#include "Application/Utility/ApplicationGlobalValue.h"

ScreenUI::ScreenUI(
    CoreEngine::BaseScene* baseScene, 
    Player* player, Stopwatch* stopwatch,
    BallController* ball, MenuController* menuController) {
    baseScene_ = baseScene;
    player_ = player;
    stopwatch_ = stopwatch;
    ball_ = ball;
    menuController_ = menuController;
}

void ScreenUI::Initialize() {
    // このシーンで使うスプライトオブジェクトの作成
    spriteObjects_.clear();
    spriteObjects_["BaseFrame"] = baseScene_->CreateObject<CoreEngine::SpriteObject>();
    spriteObjects_["BaseFrame"]->Initialize("Texture/UI_baseframe.png", "BaseUIFrame");
    spriteObjects_["BaseFrame"]->GetSpriteTransform().scale = { 1.05f,1.05f,1.05f };
    spriteObjects_["SubFrame"] = baseScene_->CreateObject<CoreEngine::SpriteObject>();
    spriteObjects_["SubFrame"]->Initialize("Texture/UI_subframe.png", "SubUIFrame");
    spriteObjects_["SubFrame"]->GetSpriteTransform().scale = { 1.05f,1.05f,1.05f };
    spriteObjects_["HandFrame"] = baseScene_->CreateObject<CoreEngine::SpriteObject>();
    spriteObjects_["HandFrame"]->Initialize("Texture/UI_handframe.png", "HandUIFrame");
    spriteObjects_["HandFrame"]->GetSpriteTransform().scale = { 1.05f,1.05f,1.05f };
    spriteObjects_["TimeLimit"] = baseScene_->CreateObject<CoreEngine::SpriteObject>();
    spriteObjects_["TimeLimit"]->Initialize("Texture/UI_timeLimit.png", "TimeLimitUI");
    spriteObjects_["TimeLimit"]->GetSpriteTransform().scale = { 1.0f,1.0f,1.0f };

    // 操作ガイド
    spriteObjects_["MenuGuide"] = baseScene_->CreateObject<CoreEngine::SpriteObject>();
    spriteObjects_["MenuGuide"]->Initialize("Texture/UI_menuGuide.png", "MenuGuideUI");
    spriteObjects_["MenuGuide"]->GetSpriteTransform().scale = { 1.0f,1.0f,1.0f };
    spriteObjects_["MoveGuide"] = baseScene_->CreateObject<CoreEngine::SpriteObject>();
    spriteObjects_["MoveGuide"]->Initialize("Texture/UI_moveGuide.png", "MoveGuideUI");
    spriteObjects_["MoveGuide"]->GetSpriteTransform().scale = { 1.0f,1.0f,1.0f };
    spriteObjects_["ShotGuide"] = baseScene_->CreateObject<CoreEngine::SpriteObject>();
    spriteObjects_["ShotGuide"]->Initialize("Texture/UI_shotGuide.png", "ShotGuideUI");
    spriteObjects_["ShotGuide"]->GetSpriteTransform().scale = { 1.0f,1.0f,1.0f };
    spriteObjects_["SwapGuide"] = baseScene_->CreateObject<CoreEngine::SpriteObject>();
    spriteObjects_["SwapGuide"]->Initialize("Texture/UI_swapGuide.png", "SwapGuideUI");
    spriteObjects_["SwapGuide"]->GetSpriteTransform().scale = { 1.0f,1.0f,1.0f };

    spriteObjects_["GunGuide"] = baseScene_->CreateObject<CoreEngine::SpriteObject>();
    spriteObjects_["GunGuide"]->Initialize("Texture/UI_gun.png", "GunGuideUI");
    spriteObjects_["GunGuide"]->GetSpriteTransform().scale = { 1.0f,1.0f,1.0f };
    spriteObjects_["YoyoGuide"] = baseScene_->CreateObject<CoreEngine::SpriteObject>();
    spriteObjects_["YoyoGuide"]->Initialize("Texture/UI_yoyo.png", "GunGuideUI");
    spriteObjects_["YoyoGuide"]->GetSpriteTransform().scale = { 1.0f,1.0f,1.0f };

    // 時間表示用ヨーヨー
    spriteObjects_["YoYoTargetPos"] = baseScene_->CreateObject<CoreEngine::SpriteObject>();
    spriteObjects_["YoYoTargetPos"]->Initialize("Texture/yoyoTargetPos.png", "YoYoTargetPosUI");
    spriteObjects_["YoYo"] = baseScene_->CreateObject<CoreEngine::SpriteObject>();
    spriteObjects_["YoYo"]->Initialize("Texture/yoyo.png", "YoYoUI");

    spriteObjects_["Bullet1"] = baseScene_->CreateObject<CoreEngine::SpriteObject>();
    spriteObjects_["Bullet1"]->Initialize("Texture/yoyoBullet_In.png", "BulletUI1");
    spriteObjects_["Bullet2"] = baseScene_->CreateObject<CoreEngine::SpriteObject>();
    spriteObjects_["Bullet2"]->Initialize("Texture/yoyoBullet_In.png", "BulletUI2");
    spriteObjects_["Bullet2"]->GetSpriteTransform().rotate.z = 3.14f * 0.5f;
    spriteObjects_["Bullet3"] = baseScene_->CreateObject<CoreEngine::SpriteObject>();
    spriteObjects_["Bullet3"]->Initialize("Texture/yoyoBullet_In.png", "BulletUI3");
    spriteObjects_["Bullet3"]->GetSpriteTransform().rotate.z = 3.14f;
    spriteObjects_["Bullet4"] = baseScene_->CreateObject<CoreEngine::SpriteObject>();
    spriteObjects_["Bullet4"]->Initialize("Texture/yoyoBullet_In.png", "BulletUI4");
    spriteObjects_["Bullet4"]->GetSpriteTransform().rotate.z = 3.14f * 1.5f;

    // 手
    spriteObjects_["Hand"] = baseScene_->CreateObject<CoreEngine::SpriteObject>();
    spriteObjects_["Hand"]->Initialize("Texture/hand.png", "HandUI");


    isOldActiveBall_ = ball_->GetIsThrowing();
    previousPlayerMode_ = player_->GetPlayerMode();

    frameTimer_ = 0.0f;
}

void ScreenUI::Update() {
    frameTimer_ += 1.0f;

    float horizontalAxis =
        player_->GetMoveAxis().x;
    float verticalAxis =
        player_->GetMoveAxis().y;

    float velocity = CoreEngine::Math::Vector::Length(player_->GetVelocity()) * 10.0f;

    // shotとswapの入れ替え
    if (previousPlayerMode_ != player_->GetPlayerMode()) {
        // モードチェンジした直後は両方下げる
        spriteObjects_["ShotGuide"]->GetSpriteTransform().translate.y = -80.0f;
        spriteObjects_["SwapGuide"]->GetSpriteTransform().translate.y = -80.0f;
    }
    if (player_->GetPlayerMode() == PlayerMode::Gun) {
        // 残弾数に応じてyoyoを回転
        spriteObjects_["YoYo"]->GetSpriteTransform().rotate.z = MatsumotoUtility::SimpleEaseIn(
            spriteObjects_["YoYo"]->GetSpriteTransform().rotate.z,
            3.14f * 0.5f + (-3.14f * 2.0f) * (static_cast<float>(ball_->GetBulletCount()) / static_cast<float>(ball_->GetMaxBulletCount())),
            0.1f);

        spriteObjects_["GunGuide"]->SetActive(true);
        spriteObjects_["YoyoGuide"]->SetActive(false);

        spriteObjects_["ShotGuide"]->SetActive(true);
        spriteObjects_["SwapGuide"]->SetActive(false);

        // 弾があるならショットガイド表示、無いなら両方下げる
        if (ball_->GetBulletCount() > 0) {
            spriteObjects_["ShotGuide"]->GetSpriteTransform().translate.y =
                MatsumotoUtility::SimpleEaseIn(
                    spriteObjects_["ShotGuide"]->GetSpriteTransform().translate.y,
                    0.0f,
                    0.1f);
            // 下げる
            if (player_->shootingBullet_) {
                spriteObjects_["ShotGuide"]->GetSpriteTransform().translate.y = -30.0f;
            }

        } else {

            spriteObjects_["ShotGuide"]->GetSpriteTransform().translate.y =
                MatsumotoUtility::SimpleEaseIn(
                    spriteObjects_["ShotGuide"]->GetSpriteTransform().translate.y,
                    -80.0f,
                    0.5f);
        }

    } else {

        spriteObjects_["GunGuide"]->SetActive(false);
        spriteObjects_["YoyoGuide"]->SetActive(true);

        if (ball_->GetIsThrowing()) {
            if (ball_->GetIsReturning()) {
                spriteObjects_["YoYo"]->GetSpriteTransform().rotate.z += 1.0f;
            } else {
                spriteObjects_["YoYo"]->GetSpriteTransform().rotate.z += 0.5f;
            }

            if (!ball_->GetIsCanSwitch()) {
                // 引き戻し不可なら両方すごい下に下げる
                spriteObjects_["ShotGuide"]->GetSpriteTransform().translate.y =
                    MatsumotoUtility::SimpleEaseIn(
                        spriteObjects_["ShotGuide"]->GetSpriteTransform().translate.y,
                        -80.0f,
                        0.5f);
                spriteObjects_["SwapGuide"]->GetSpriteTransform().translate.y =
                    MatsumotoUtility::SimpleEaseIn(
                        spriteObjects_["SwapGuide"]->GetSpriteTransform().translate.y,
                        -80.0f,
                        0.5f);

            } else {
                // 引き戻し可能ならスワップガイド表示
                spriteObjects_["ShotGuide"]->SetActive(false);
                spriteObjects_["SwapGuide"]->SetActive(true);
            }
        } else {
            spriteObjects_["YoYo"]->GetSpriteTransform().rotate.z += 0.1f;

            spriteObjects_["ShotGuide"]->SetActive(true);
            spriteObjects_["SwapGuide"]->SetActive(false);
        }
    }

    // 回転角度の正規化
    if (spriteObjects_["YoYo"]->GetSpriteTransform().rotate.z > 3.14f * 2.0f) {
        spriteObjects_["YoYo"]->GetSpriteTransform().rotate.z -= 3.14f * 2.0f;
    }

    // ボールのアクティブ状態が変化したらガイドを少し動かす
    if (isOldActiveBall_ != ball_->GetIsThrowing()) {
        spriteObjects_["ShotGuide"]->GetSpriteTransform().translate.y = -50.0f;
        spriteObjects_["SwapGuide"]->GetSpriteTransform().translate.y = -50.0f;
    }

    // プレイヤーが移動操作をしている時はMoveガイドを少し下に移動
    if (player_->GetPlayerMode() == PlayerMode::YoYo && !ball_->GetIsThrowing()){
        if (horizontalAxis != 0.0f || verticalAxis != 0.0f) {
            spriteObjects_["MoveGuide"]->GetSpriteTransform().translate.y =
                MatsumotoUtility::SimpleEaseIn(
                    spriteObjects_["MoveGuide"]->GetSpriteTransform().translate.y,
                    -30.0f,
                    0.3f);
        } else {
            spriteObjects_["MoveGuide"]->GetSpriteTransform().translate.y =
                MatsumotoUtility::SimpleEaseIn(
                    spriteObjects_["MoveGuide"]->GetSpriteTransform().translate.y,
                    0.0f,
                    0.1f);
        }
    } else {// 操作できないので思いっきり下げる
        spriteObjects_["MoveGuide"]->GetSpriteTransform().translate.y =
            MatsumotoUtility::SimpleEaseIn(
                spriteObjects_["MoveGuide"]->GetSpriteTransform().translate.y,
                -80.0f,
                0.5f);
    }

    // メニューを開いている時はMenuガイドを少し下に移動
    if (menuController_->IsMenuOpen()) {
        spriteObjects_["MenuGuide"]->GetSpriteTransform().translate.y =
            MatsumotoUtility::SimpleEaseIn(
                spriteObjects_["MenuGuide"]->GetSpriteTransform().translate.y,
                -30.0f,
                0.3f);
    } else {
        spriteObjects_["MenuGuide"]->GetSpriteTransform().translate.y =
            MatsumotoUtility::SimpleEaseIn(
                spriteObjects_["MenuGuide"]->GetSpriteTransform().translate.y,
                0.0f,
                0.1f);
    }

    // デフォルト位置に戻す
    float timeRatio = static_cast<float>(stopwatch_->ElapsedMilliseconds() / ApplicationGlobalValue::GAME_CLEAR_TIME_MS);
    spriteObjects_["YoYo"]->GetSpriteTransform().translate = { -450.0f,MatsumotoUtility::Lerp(-250.0f,100.0f,timeRatio),0.0f };
    spriteObjects_["YoYoTargetPos"]->GetSpriteTransform().translate = { -450.0f,100.0f,0.0f };
    // 時間制限が1/10だったら点滅させる
    if (timeRatio >= 0.9f) {
        CoreEngine::Vector4 color = {
            1.0f,(sinf(frameTimer_ * 0.3f) + 1.0f) * 0.5f,(sinf(frameTimer_ * 0.3f) + 1.0f) * 0.5f,
            1.0f
        };
        spriteObjects_["YoYo"]->SetColor(color);

        spriteObjects_["YoYo"]->GetSpriteTransform().scale =
        {
            MatsumotoUtility::Lerp(1.0f,1.2f,(sinf(frameTimer_ * 0.3f) + 1.0f) * 0.5f),
            MatsumotoUtility::Lerp(1.0f,1.2f,(sinf(frameTimer_ * 0.3f) + 1.0f) * 0.5f),
            1.0f
        };
    } else {
        spriteObjects_["YoYo"]->SetColor({ 1.0f,1.0f,1.0f,1.0f });
    }


    spriteObjects_["Hand"]->GetSpriteTransform().translate = { -550.0f,280.0f,0.0f };

    // UIをちょっと動かす
    float offsetAmount = 5.0f;
    for (auto& [name, spriteObject] : spriteObjects_) {
        spriteObject->GetSpriteTransform().translate.x =
            MatsumotoUtility::SimpleEaseIn(
                spriteObject->GetSpriteTransform().translate.x,
                horizontalAxis * offsetAmount + velocity,
                0.1f);
        spriteObject->GetSpriteTransform().translate.y =
            MatsumotoUtility::SimpleEaseIn(
                spriteObject->GetSpriteTransform().translate.y,
                verticalAxis * offsetAmount + velocity,
                0.1f);
        offsetAmount += 2.0f;
    }

    // 弾丸表示
    int bulletCount = ball_->GetBulletCount();
    spriteObjects_["Bullet1"]->SetActive(bulletCount >= 1);
    spriteObjects_["Bullet2"]->SetActive(bulletCount >= 2);
    spriteObjects_["Bullet3"]->SetActive(bulletCount >= 3);
    spriteObjects_["Bullet4"]->SetActive(bulletCount >= 4);

    spriteObjects_["Bullet1"]->GetSpriteTransform().translate = spriteObjects_["YoYo"]->GetSpriteTransform().translate;
    spriteObjects_["Bullet2"]->GetSpriteTransform().translate = spriteObjects_["YoYo"]->GetSpriteTransform().translate;
    spriteObjects_["Bullet3"]->GetSpriteTransform().translate = spriteObjects_["YoYo"]->GetSpriteTransform().translate;
    spriteObjects_["Bullet4"]->GetSpriteTransform().translate = spriteObjects_["YoYo"]->GetSpriteTransform().translate;

    spriteObjects_["Bullet1"]->GetSpriteTransform().rotate.z = spriteObjects_["YoYo"]->GetSpriteTransform().rotate.z; +0.0f;
    spriteObjects_["Bullet2"]->GetSpriteTransform().rotate.z = spriteObjects_["YoYo"]->GetSpriteTransform().rotate.z + 3.14f * 0.5f;
    spriteObjects_["Bullet3"]->GetSpriteTransform().rotate.z = spriteObjects_["YoYo"]->GetSpriteTransform().rotate.z + 3.14f;
    spriteObjects_["Bullet4"]->GetSpriteTransform().rotate.z = spriteObjects_["YoYo"]->GetSpriteTransform().rotate.z + 3.14f * 1.5f;

    isOldActiveBall_ = ball_->GetIsThrowing();
    previousPlayerMode_ = player_->GetPlayerMode();
}
