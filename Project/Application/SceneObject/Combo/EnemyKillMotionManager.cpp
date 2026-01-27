#include "EnemyKillMotionManager.h"
#include "Application/SceneObject/Combo/EnemyKillComboCounter.h"
#include "Application/SceneObject/Enemy/EnemyContainer.h"
#include "Application/SceneObject/Player/Player.h"
#include "Application/SceneObject/CameraController/CameraController.h"
#include "Application/SceneObject/Ball/BallController.h"

#include "Application/SceneObject/CameraController/AllCameraWork.h"

EnemyKillMotionManager::EnemyKillMotionManager(
    EnemyKillComboCounter* comboCounter,
    Player* player,
    EnemyContainer* enemyContainer,
    CameraController* cameraController,
    BallController* ballController) :
    container_(enemyContainer),
    comboCounter_(comboCounter),
    player_(player),
    cameraController_(cameraController),
    ballController_(ballController) {
    isPlayingMotion_ = false;
    eraseCooldown_ = 0.5f;
    currentEraseCooldown_ = 0.0f;
    eraseCooldownFactor_ = 1.0f;
}

void EnemyKillMotionManager::Update() {
#ifdef _DEBUG
    ImGui::Begin("EnemyKillMotionManager");
    ImGui::Text("isPlayingMotion_: %d", isPlayingMotion_);
    ImGui::Text("CurrentEraseCooldown_: %.2f", currentEraseCooldown_);
    ImGui::Text("ComboTimer: %.2f", comboCounter_->GetComboTimer());
    ImGui::Text("CurrentCombo: %d", comboCounter_->GetCurrentCombo());
    ImGui::End();
#endif // _DEBUG
    // コンボタイマーの更新処理
    auto enemyList = container_->DeathEnemyList();

    if (player_->isDamaged_) {
        isPlayingMotion_ = false;
        player_->isDamaged_ = false;
        cameraController_->SetCameraWork<GoToCamera>(CoreEngine::Vector3(0.0f, 24.0f, -24.0f), 0.1f);
        // 全ての死んだ敵を非アクティブ化
        for (auto enemy : enemyList) {
            enemy->SetActive(false);
            if (killEffectFunc_) {
                killEffectFunc_(
                    enemy->GetTransform(), 
                    CoreEngine::Vector3(0.0f,0.0f,0.0f),
                    CoreEngine::Vector3(0.3f, 0.3f, 0.3f));
            }
        }
        return;
    }
    
    // 死んだ敵がいる状態でプレイヤーがボールを回収した場合キル演出開始
    if (!enemyList.empty() && !ballController_->GetIsThrowing()) {
        if (!isPlayingMotion_) {
            isPlayingMotion_ = true;
            currentEraseCooldown_ = static_cast<float>(enemyList.size())*0.05f; // 最初の消去までの猶予
            eraseCooldownFactor_ = 1.0f;
        }
    }

    // キル演出中の更新処理
    if (isPlayingMotion_) {
        if (!enemyList.empty()) {
            // 一番遠い敵を取得
            float maxDistance = 0.0f;
            CoreEngine::Vector3 furthestEnemyPos;
            IEnemy* furthestEnemy = nullptr;
            for (auto enemy : enemyList) {
                CoreEngine::Vector3 toEnemy = enemy->GetTransform() - player_->GetTransform();
                float distance = CoreEngine::Math::Vector::Length(toEnemy);
                if (distance > maxDistance) {
                    maxDistance = distance;
                    furthestEnemyPos = enemy->GetTransform();
                    furthestEnemy = enemy;
                }
            }
            // カメラワーク設定
            cameraController_->SetCameraWork<TowPointFramingCameraWork>(furthestEnemy->GetTransform() ,player_->GetTransform(), 0.1f);

            // 外側のやつから順番に消す
            if (currentEraseCooldown_ <= 0.0f) {
                furthestEnemy->SetActive(false);
                currentEraseCooldown_ = eraseCooldown_ * eraseCooldownFactor_;
                eraseCooldownFactor_ *= 0.7f; // 徐々に速くする
                // エフェクトの再生
                if (killEffectFunc_) {
                    killEffectFunc_(
                        furthestEnemy->GetTransform(),
                        CoreEngine::Vector3(0.0f, 0.0f, 0.0f),
                        CoreEngine::Vector3(0.3f, 0.3f, 0.3f));
                }

            } else {
                currentEraseCooldown_ -= 1.0f / 60.0f;
            }

            return;
        } else {
            isPlayingMotion_ = false;
            cameraController_->ResetDefaultCameraWork();
        }
    }


}

