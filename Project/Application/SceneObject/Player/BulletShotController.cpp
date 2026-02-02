#include "BulletShotController.h"
#include "Application/SceneObject/Ball/Ball.h"
#include "Application/SceneObject/Player/Player.h"

#include "Application/Utility/KeyBindConfig.h"
#include "Application/Utility/MatsumotoUtility.h"

BulletShotController::BulletShotController(Player* player, Ball* ball) {
    player_ = player;
    ball_ = ball;
}

void BulletShotController::Update(BulletObjectContainer* bulletContainer) {
    // ボールがアクティブじゃなくて、プレイヤーがガンモードのときに弾を発射
    bool canShoot = !ball_->IsActive() && player_->GetPlayerMode() == PlayerMode::Gun;

    KeyBindConfig& keyBindConfig = KeyBindConfig::Instance();
    if (keyBindConfig.IsTrigger("Shot") && canShoot) {
        // プレイヤーの向きから弾の発射方向を決定
        CoreEngine::Vector3 shootDir = player_->lookDir_;
        // 弾オブジェクトをコンテナから取得して初期化
        int result =
            bulletContainer->Spawn(
                player_->GetWorldPosition(),
                MatsumotoUtility::DirectionToEulerAngle(shootDir),
                CoreEngine::Vector3(1.0f, 1.0f, 1.0f));
        result;
        // 発射音などの効果音を再生する場合はここで行う
        /*if (result != -1) {
            player_->PlaySE("gun_shot");
        }*/
    }
}
