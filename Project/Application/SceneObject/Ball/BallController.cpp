#include "BallController.h"

#include "Application/SceneObject/Player/Player.h"
#include "Application/SceneObject/Ball/Ball.h"
#include "Application/Utility/StringRenderer.h"
#include "Engine/Scene/BaseScene.h"

#include "Application/Utility/KeyBindConfig.h"
#include "Application/Utility/MatsumotoUtility.h"

namespace {
    float deltaTime = 1.0f / 60.0f; // 仮のデルタタイム
}

BallController::BallController(Ball* ball, Player* player, CoreEngine::BaseScene* scene) :
    player_(player),
    ball_(ball) {

    // ... (config initialization)
    // 17-30
    config_.emplace("ShotSpeed", 0.1f);
    config_.emplace("MoveSpeed", 0.1f);
    config_.emplace("ReturnSpeed", 0.1f);
    config_.emplace("ShotRadius", 11.0f);
    config_.emplace("HangTime", 1.5f);
    config_.emplace("SwitchCooldown", 0.2f);
    config_.emplace("ReturnThreshold", 0.0f);
    config_.emplace("SteerStrength", 8.0f);
    config_.emplace("CatchRadius", 3.0f);
    config_.emplace("Obedience", 30.0f);
    config_.emplace("ReturnSteerStrength", 30.0f);
    config_.emplace("ReturnImpulse", 50.0f);

    canSwitch_ = true;
    isReturning_ = false;
    wasReturning_ = false;
    nowRadius_ = 0.0f;
    switchCooldown_ = 0.0f;

    // StringRendererをシーンに登録
    stringRenderer_ = scene->CreateObject<StringRenderer>();
    stringRenderer_->Initialize(16);
    stringRenderer_->SetAutoUpdate(false); // 手動で更新するため自動更新をオフ
}

BallController::~BallController() {}

void BallController::Initialize() {
    MatsumotoUtility::LoadSceneObjectConfig(config_, "BallControllerConfig.json");
    ballVelocity_ = { 0.0f, 0.0f, 0.0f };
}

void BallController::Update() {
    if (!ball_ || !player_) {
        return;
    }
    // クールダウン更新
    if (switchCooldown_ > 0.0f) {
        switchCooldown_ -= deltaTime;
    }

    // アンカーポイント更新（常にプレイヤーに追従させる）
    anchorPos_ = player_->GetWorldPosition();

    // プレイヤーがダメージを受けたら強制引き戻し
    if (player_->GetDamageInvincibilityTime() > 0.0f) {
        isReturning_ = true;
    }

    if (ball_->IsActive()) {
        // 減速処理
        ballVelocity_ *= 0.98f;
    }

    // 発射処理
    if (KeyBindConfig::Instance().IsTrigger("Shot")) {
        if (!ball_->IsActive()) {// 球が出ていなければ発射
            // プレイヤーがダメージ無敵時間内に攻撃しようとしたらすぐに無敵時間を終了させる
            if (player_->GetDamageInvincibilityTime() > 0.0f) {
                player_->SetDamageInvincibilityTime(0.0f);
            }

            ball_->SetActive(true);
            anchorPos_ = player_->GetWorldPosition();
            isReturning_ = false;
            hangTimeCounter_ = config_["HangTime"].get<float>();
            nowRadius_ = 0.0f;
            player_->canMove_ = false;
            ballVelocity_ = { 0.0f, 0.0f, 0.0f };

            // プレイヤーの向きにボールをセット
            CoreEngine::Vector3 playerLookDir = player_->lookDir_;
            cartesianPos_.y = atan2f(playerLookDir.x, playerLookDir.z);
            cartesianPos_.z = acosf(playerLookDir.y / 1.0f); // 半径1で正規化されているので
            CoreEngine::Vector3 newPos = MatsumotoUtility::SphericalToCartesian(0.1f, cartesianPos_.y, cartesianPos_.z);
            ball_->GetTransform() = (anchorPos_ + newPos + ballVelocity_);
            player_->PlaySE("throw");

        } else {// 球が出ていれば引き戻し
            if (canSwitch_ && switchCooldown_ <= 0.0f) {
                isReturning_ = true;
                switchCooldown_ = config_["SwitchCooldown"].get<float>();
                canSwitch_ = false;
            }
        }
    }
    // 発射後の挙動
    if (ball_->IsActive()) {
        ball_->rotateSpeed_ = hangTimeCounter_ * 0.3f;

        // プレイヤーの向きにボールを追従させる
        CoreEngine::Vector3 playerLookDir = player_->lookDir_;
        cartesianPos_.y = MatsumotoUtility::SimpleEaseInAngle(cartesianPos_.y, atan2f(playerLookDir.x, playerLookDir.z), config_["MoveSpeed"].get<float>());
        cartesianPos_.z = MatsumotoUtility::SimpleEaseInAngle(cartesianPos_.z, acosf(playerLookDir.y / 1.0f), config_["MoveSpeed"].get<float>());

        // 引き戻し中かどうかで処理を分岐
        if (isReturning_) {
            // 引き戻しが始まった瞬間なら、プレイヤーへ向かう強い初速を与える
            if (!wasReturning_) {
                CoreEngine::Vector3 toPlayer = CoreEngine::Math::Vector::Normalize(player_->GetWorldPosition() - ball_->GetWorldPosition());
                ball_->SetVelocity(toPlayer * config_["ReturnImpulse"].get<float>());
            }

            // 引き戻し処理
            nowRadius_ = MatsumotoUtility::SimpleEaseIn(nowRadius_, 0.0f, config_["ReturnSpeed"].get<float>());
            CoreEngine::Vector3 newPos = MatsumotoUtility::SphericalToCartesian(nowRadius_, cartesianPos_.y, cartesianPos_.z);
            CoreEngine::Vector3 targetPos = (anchorPos_ + newPos);
            CoreEngine::Vector3 currentVel = ball_->GetVelocity();
            CoreEngine::Vector3 desiredVel = (targetPos - ball_->GetWorldPosition()) * config_["ReturnSteerStrength"].get<float>();
            CoreEngine::Vector3 steer = desiredVel - currentVel;
            ball_->SetVelocity(currentVel + steer * deltaTime);

            // プレイヤーとの距離を計算
            float distance = CoreEngine::Math::Vector::Length(ball_->GetWorldPosition() - player_->GetWorldPosition());
            // プレイヤーに十分に到達したら非アクティブ化
            if (distance <= config_["CatchRadius"].get<float>()) {
                player_->canMove_ = true;
                ball_->SetActive(false);
                ball_->SetVelocity({ 0.0f, 0.0f, 0.0f });
            }
        } else {
            // 引き戻し時間待機
            if (hangTimeCounter_ > 0.0f) {
                hangTimeCounter_ -= deltaTime;
            } else {
                isReturning_ = true;
            }

            // 半径拡大
            nowRadius_ = MatsumotoUtility::SimpleEaseIn(nowRadius_, config_["ShotRadius"].get<float>(), config_["ShotSpeed"].get<float>());
            CoreEngine::Vector3 newPos = MatsumotoUtility::SphericalToCartesian(nowRadius_, cartesianPos_.y, cartesianPos_.z);
            CoreEngine::Vector3 targetPos = (anchorPos_ + newPos);
            CoreEngine::Vector3 currentVel = ball_->GetVelocity();
            CoreEngine::Vector3 desiredVel = (targetPos - ball_->GetWorldPosition()) * config_["Obedience"].get<float>();
            CoreEngine::Vector3 steer = desiredVel - currentVel;
            ball_->SetVelocity(currentVel + steer * deltaTime);
        }

        wasReturning_ = isReturning_;
    } else {
        canSwitch_ = true;
        isReturning_ = false;
        wasReturning_ = false;
    }

    // ひもの更新
    if (ball_->IsActive()) {
        stringRenderer_->Update(player_->GetWorldPosition(), ball_->GetWorldPosition(), player_->lookDir_, ball_->GetVelocity());
        stringRenderer_->SetActive(true);
    } else {
        stringRenderer_->SetActive(false);
    }

    // ボールが敵に当たったら引き戻しor反射
    if (ball_->isHitEnemy_) {
        ball_->PlaySE("Hit");
        // ヒット時の衝撃を直接速度に加える（現在の速度を維持しつつ跳ね返る）
        CoreEngine::Vector3 impulse = -ball_->GetMoveDir() * 40.0f;
        ball_->SetVelocity(impulse);

        if (hitEffectFunc_) {
            CoreEngine::Vector3 direction = -ball_->GetMoveDir();
            CoreEngine::Vector3 rotate = MatsumotoUtility::DirectionToEulerAngle(direction);
            hitEffectFunc_(
                ball_->hitPos_,
                rotate,
                CoreEngine::Vector3(1.0f, 1.0f, 1.0f));
        }
        if (slashEffectFunc_) {
            CoreEngine::Vector3 direction = ball_->GetMoveDir();
            CoreEngine::Vector3 rotate = MatsumotoUtility::DirectionToEulerAngle(direction);
            rotate.x = 0.0f;
            rotate.z = 0.0f;
            slashEffectFunc_(
                ball_->hitPos_,
                rotate,
                CoreEngine::Vector3(5.0f, 0.1f, 50.0f));
        }
        // 反射処理
        isReturning_ = false;
        hangTimeCounter_ = config_["HangTime"].get<float>();
        ball_->isHitEnemy_ = false;
        canSwitch_ = true;
    }
}
void BallController::Draw(const CoreEngine::ICamera* camera) {
    (void)camera;
}

bool BallController::GetIsThrowing() {
    return ball_->IsActive();
}

bool BallController::GetIsCanSwitch() {
    return canSwitch_;
}
