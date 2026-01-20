#include "BallController.h"

#include "Application/SceneObject/Player/Player.h"
#include "Application/SceneObject/Ball/Ball.h"

#include "Application/Utility/KeyBindConfig.h"
#include "Application/Utility/MatsumotoUtility.h"

namespace {
    float deltaTime = 1.0f / 60.0f; // 仮のデルタタイム
}

BallController::BallController(Ball* ball, Player* player) :
    player_(player),
    ball_(ball) {

    config_.emplace("ShotSpeed", 0.1f);
    config_.emplace("MoveSpeed", 0.1f);
    config_.emplace("ReturnSpeed", 0.1f);
    config_.emplace("ShotRadius", 5.0f);
    config_.emplace("HangTime", 1.5f);
    config_.emplace("SwitchCooldown", 0.2f);

    isReturning_ = false;
    nowRadius_ = 0.0f;
    switchCooldown_ = 0.0f;
}

void BallController::Update() {
    if (!ball_ || !player_) {
        return;
    }
    // クールダウン更新
    if (switchCooldown_ > 0.0f) {
        switchCooldown_ -= deltaTime;
    }

    // 発射処理
    if (KeyBindConfig::Instance().IsTrigger("Shot")) {
        if (!ball_->IsActive()) {// 球が出ていなければ発射
            ball_->SetActive(true);
            anchorPos_ = player_->GetWorldPosition();
            isReturning_ = false;
            hangTimeCounter_ = config_["HangTime"].get<float>();
            nowRadius_ = 0.0f;
            player_->canMove_ = false;

            // プレイヤーの向きにボールをセット
            CoreEngine::Vector3 playerLookDir = player_->lookDir_;
            cartesianPos_.y = atan2f(playerLookDir.x, playerLookDir.z);
            cartesianPos_.z = acosf(playerLookDir.y / 1.0f); // 半径1で正規化されているので
            CoreEngine::Vector3 newPos = MatsumotoUtility::SphericalToCartesian(0.1f, cartesianPos_.y, cartesianPos_.z);
            ball_->GetTransform() = (anchorPos_ + newPos);
        } else {// 球が出ていればプレイヤーと球の位置を変えてスイッチ
            if (switchCooldown_ <= 0.0f) {
                CoreEngine::Vector3 ballPos = ball_->GetWorldPosition();
                CoreEngine::Vector3 playerPos = player_->GetWorldPosition();
                // ボールとプレイヤーの位置を入れ替え
                ball_->GetTransform() = playerPos;
                player_->GetTransform() = ballPos;
                // 引き戻し処理をリセット
                anchorPos_ = ball_->GetWorldPosition();
                isReturning_ = false;
                hangTimeCounter_ = config_["HangTime"].get<float>();
                switchCooldown_ = config_["SwitchCooldown"].get<float>();

                player_->lookDir_.x = -CoreEngine::Math::Vector::Normalize((ballPos - playerPos)).z;
                player_->lookDir_.z = -CoreEngine::Math::Vector::Normalize((ballPos - playerPos)).x;
                CoreEngine::Vector3 playerLookDir = player_->lookDir_;
                cartesianPos_.y = atan2f(playerLookDir.x, playerLookDir.z);
                cartesianPos_.z = acosf(playerLookDir.y / 1.0f);
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
            // 引き戻し処理
            nowRadius_ = MatsumotoUtility::SimpleEaseIn(nowRadius_, 0.0f, config_["ReturnSpeed"].get<float>());
            CoreEngine::Vector3 newPos = MatsumotoUtility::SphericalToCartesian(nowRadius_, cartesianPos_.y, cartesianPos_.z);
            ball_->GetTransform() = (anchorPos_ + newPos);
            // プレイヤーに到達したら非アクティブ化
            if (nowRadius_ <= 0.3f) {
                player_->canMove_ = true;
                ball_->SetActive(false);
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
            ball_->GetTransform() = (anchorPos_ + newPos);
        }
    }

    // ボールが敵に当たったら引き戻しor反射
    if (ball_->isHitEnemy_) {
        // ボールより敵が近いなら反射
        CoreEngine::Vector3 ballPos = ball_->GetWorldPosition();
        CoreEngine::Vector3 hitPos = ball_->hitPos_;
        if ((CoreEngine::Math::Vector::Length(ballPos - hitPos) < nowRadius_)) {
            // 反射処理
            isReturning_ = false;
            hangTimeCounter_ = config_["HangTime"].get<float>();

        } else {
            // 引き戻し処理
            isReturning_ = true;
            hangTimeCounter_ = 0.0f;
        }
        ball_->isHitEnemy_ = false;
    }
}
