#include "GameCameraController.h"
#include "Engine/Camera/ICamera.h"
#include "Engine/Camera/Release/Camera.h"
#include "Engine/EngineSystem/EngineSystem.h"
#include "Engine/Utility/FrameRate/FrameRateController.h"
#include "Application/AL3/GameObject/PlayerObject.h"
#include "Application/AL3/GameObject/BossObject.h"

void GameCameraController::Initialize(EngineSystem* engine, ICamera* camera, PlayerObject* player, BossObject* boss) {
    engine_ = engine;
    camera_ = camera;
    player_ = player;
    boss_ = boss;

    // タイマーの初期化
    bossFocusTimer_.SetName("BossFocusTimer");
    moveToPlayerTimer_.SetName("MoveToPlayerTimer");
    bossDefeatTimer_.SetName("BossDefeatTimer");
    playerDeathTimer_.SetName("PlayerDeathTimer");

    // 初期状態はボス登場演出
    currentState_ = CameraState::OpeningBossFocus;
}

void GameCameraController::Update() {
    if (!camera_ || !player_ || !boss_) return;

    auto frameRateController = engine_->GetComponent<FrameRateController>();
    if (!frameRateController) return;

    float deltaTime = frameRateController->GetDeltaTime();

    // タイマーの更新
    bossFocusTimer_.Update(deltaTime);
    moveToPlayerTimer_.Update(deltaTime);
    bossDefeatTimer_.Update(deltaTime);
    playerDeathTimer_.Update(deltaTime);

    // 状態に応じた更新処理
    switch (currentState_) {
    case CameraState::OpeningBossFocus:
        UpdateOpeningBossFocus();
        break;
    case CameraState::OpeningToPlayer:
        UpdateOpeningToPlayer();
        break;
    case CameraState::FollowPlayer:
        UpdateFollowPlayer();
        break;
    case CameraState::BossDefeatFocus:
        UpdateBossDefeatFocus();
        break;
    case CameraState::PlayerDeathFocus:
        UpdatePlayerDeathFocus();
        break;
    }
}

void GameCameraController::StartOpeningSequence() {
    // ボス登場演出を開始
    currentState_ = CameraState::OpeningBossFocus;

    // ボス登場演出タイマーを開始
    bossFocusTimer_.Start(bossFocusDuration_, false);

    // タイマー終了時に次の状態へ遷移
    bossFocusTimer_.SetOnComplete([this]() {
        // プレイヤーへの移動演出に移行
        currentState_ = CameraState::OpeningToPlayer;

        // 現在のカメラ位置と注視点を保存
        transitionStartPosition_ = camera_->GetPosition();
        Vector3 bossPosition = boss_->GetTransform().translate;
        transitionStartTarget_ = MathCore::Vector::Add(bossPosition, bossOpeningCameraTargetOffset_);

        // プレイヤーへの移動タイマーを開始
        moveToPlayerTimer_.Start(moveToPlayerDuration_, false);

        // 移動終了時にプレイヤー追従に切り替え
        moveToPlayerTimer_.SetOnComplete([this]() {
            currentState_ = CameraState::FollowPlayer;

            // 演出終了コールバックを呼び出し
            if (onOpeningFinishedCallback_) {
                onOpeningFinishedCallback_();
            }
            });
        });
}

void GameCameraController::UpdateOpeningBossFocus() {
    // ボスを遠くから見下ろす
    Vector3 bossPosition = boss_->GetTransform().translate;
    Vector3 cameraPosition = MathCore::Vector::Add(bossPosition, bossOpeningCameraOffset_);

    // ボスの上半身を注視
    Vector3 targetPosition = MathCore::Vector::Add(bossPosition, bossOpeningCameraTargetOffset_);

    SetCameraPositionAndTarget(cameraPosition, targetPosition);
}

void GameCameraController::UpdateOpeningToPlayer() {
    // プレイヤーへの移動をイージング補間
    float t = moveToPlayerTimer_.GetElapsedTime() / moveToPlayerDuration_;

    // 目標位置と注視点を計算
    Vector3 playerPosition = player_->GetTransform().translate;
    Vector3 targetCameraPosition = MathCore::Vector::Add(playerPosition, playerCameraOffset_);
    Vector3 targetLookAt = MathCore::Vector::Add(playerPosition, Vector3{ 0.0f, 1.0f, 0.0f });

    // イージングを適用して補間
    Vector3 currentPosition = EasingUtil::LerpVector3(
        transitionStartPosition_,
        targetCameraPosition,
        t,
        easingType_
    );

    Vector3 currentTarget = EasingUtil::LerpVector3(
        transitionStartTarget_,
        targetLookAt,
        t,
        easingType_
    );

    SetCameraPositionAndTarget(currentPosition, currentTarget);
}

void GameCameraController::UpdateFollowPlayer() {
    // プレイヤーを追従しながらボスも視界に入れ続ける
    auto frameRateController = engine_->GetComponent<FrameRateController>();
    if (!frameRateController) return;

    float deltaTime = frameRateController->GetDeltaTime();

    // プレイヤーとボスの位置を取得
    Vector3 playerPosition = player_->GetTransform().translate;
    Vector3 bossPosition = boss_->GetTransform().translate;

    // プレイヤーからボスへの方向ベクトル
    Vector3 playerToBoss = MathCore::Vector::Subtract(bossPosition, playerPosition);
    Vector3 playerToBossDir = MathCore::Vector::Normalize(playerToBoss);

    // カメラをプレイヤーの後ろ側に配置（ボスの反対方向）
    Vector3 cameraDirection = MathCore::Vector::Multiply(-1.0f, playerToBossDir);

    // カメラの高さと距離を取得
    float cameraDistance = MathCore::Vector::Length(Vector3{ playerCameraOffset_.x, 0.0f, playerCameraOffset_.z });
    float cameraHeight = playerCameraOffset_.y;

    // カメラの水平方向のオフセットを計算（ボスの反対方向に配置）
    Vector3 horizontalOffset = MathCore::Vector::Multiply(cameraDistance, cameraDirection);
    horizontalOffset.y = 0.0f; // 水平方向のみ

    // カメラの最終位置を計算（プレイヤーの後ろ + 高さ）
    Vector3 targetCameraPosition = playerPosition;
    targetCameraPosition = MathCore::Vector::Add(targetCameraPosition, horizontalOffset);
    targetCameraPosition.y += cameraHeight;

    // プレイヤーとボスの中間地点を計算（注視点）
    Vector3 midPoint = MathCore::Vector::Add(playerPosition, MathCore::Vector::Multiply(0.5f, playerToBoss));

    // 注視点の高さを調整（プレイヤーとボスの中心あたり）
    Vector3 targetLookAt = MathCore::Vector::Add(midPoint, Vector3{ 0.0f, 1.0f, 0.0f });

    // 現在のカメラ位置から目標位置へ滑らかに移動
    Vector3 currentPosition = camera_->GetPosition();
    Vector3 newPosition = EasingUtil::LerpVector3(
        currentPosition,
        targetCameraPosition,
        cameraFollowSpeed_ * deltaTime,
        EasingUtil::Type::Linear
    );

    SetCameraPositionAndTarget(newPosition, targetLookAt);

    // カメラの方向ベクトルをキャッシュ（入力処理で使用）
    Vector3 cameraToTarget = MathCore::Vector::Subtract(targetLookAt, newPosition);
    cameraToTarget.y = 0.0f; // XZ平面に投影
    currentCameraForward_ = MathCore::Vector::Normalize(cameraToTarget);

    // 右方向は前方向に対して垂直（外積を使用）
    Vector3 up = { 0.0f, 1.0f, 0.0f };
    currentCameraRight_ = MathCore::Vector::Normalize(MathCore::Vector::Cross(up, currentCameraForward_));
}

void GameCameraController::SetCameraPositionAndTarget(const Vector3& position, const Vector3& target) {
    // Camera型にキャスト（ICamera -> Camera）
    Camera* camera = static_cast<Camera*>(camera_);
    if (!camera) return;

    // カメラの位置を設定
    camera->SetTranslate(position);

    // LookAt行列を計算してビュー行列として設定
    Vector3 up = { 0.0f, 1.0f, 0.0f };
    Matrix4x4 viewMatrix = MathCore::Matrix::LookAt(position, target, up);
    camera->SetViewMatrix(viewMatrix);

    // カメラの行列を更新
    camera->UpdateMatrix();
    camera->TransferMatrix();
}

void GameCameraController::StartBossDefeatSequence() {
    // ボス撃破演出を開始
    currentState_ = CameraState::BossDefeatFocus;

    // 現在のカメラ位置と注視点を保存
    transitionStartPosition_ = camera_->GetPosition();
    Vector3 playerPosition = player_->GetTransform().translate;
    transitionStartTarget_ = MathCore::Vector::Add(playerPosition, Vector3{ 0.0f, 1.0f, 0.0f });

    // ボス撃破タイマーを開始
    bossDefeatTimer_.Start(bossDefeatFocusDuration_, false);

    // タイマー終了時のコールバック
    bossDefeatTimer_.SetOnComplete([this]() {
        // 撃破演出終了コールバックを呼び出し
        if (onBossDefeatFinishedCallback_) {
            onBossDefeatFinishedCallback_();
        }
        });
}

void GameCameraController::UpdateBossDefeatFocus() {
    // ボスにフォーカスする位置へイージング補間で移動
    float t = bossDefeatTimer_.GetElapsedTime() / bossDefeatFocusDuration_;

    // 目標位置と注視点を計算
    Vector3 bossPosition = boss_->GetTransform().translate;
    Vector3 targetCameraPosition = MathCore::Vector::Add(bossPosition, bossDefeatCameraOffset_);
    // ボスの中心（高さ2.0の位置）を注視
    Vector3 targetLookAt = MathCore::Vector::Add(bossPosition, Vector3{ 0.0f, 1.5f, 0.0f });

    // イージングを適用して補間
    Vector3 currentPosition = EasingUtil::LerpVector3(
        transitionStartPosition_,
        targetCameraPosition,
        t,
        easingType_
    );

    Vector3 currentTarget = EasingUtil::LerpVector3(
        transitionStartTarget_,
        targetLookAt,
        t,
        easingType_
    );

    SetCameraPositionAndTarget(currentPosition, currentTarget);
}

bool GameCameraController::IsBossDefeatFinished() const {
    return currentState_ == CameraState::BossDefeatFocus && !bossDefeatTimer_.IsActive();
}

void GameCameraController::StartPlayerDeathSequence() {
    // プレイヤー死亡演出を開始
    currentState_ = CameraState::PlayerDeathFocus;

    // 現在のカメラ位置と注視点を保存
    transitionStartPosition_ = camera_->GetPosition();
    Vector3 playerPosition = player_->GetTransform().translate;
    transitionStartTarget_ = MathCore::Vector::Add(playerPosition, Vector3{ 0.0f, 1.0f, 0.0f });

    // プレイヤー死亡タイマーを開始
    playerDeathTimer_.Start(playerDeathFocusDuration_, false);

    // タイマー終了時のコールバック
    playerDeathTimer_.SetOnComplete([this]() {
        // 死亡演出終了コールバックを呼び出し
        if (onPlayerDeathFinishedCallback_) {
            onPlayerDeathFinishedCallback_();
        }
        });
}

void GameCameraController::UpdatePlayerDeathFocus() {
    // プレイヤーにフォーカスする位置へイージング補間で移動
    float t = playerDeathTimer_.GetElapsedTime() / playerDeathFocusDuration_;

    // 目標位置と注視点を計算（プレイヤーを斜めから見下ろす）
    Vector3 playerPosition = player_->GetTransform().translate;
    Vector3 targetCameraPosition = MathCore::Vector::Add(playerPosition, playerDeathCameraOffset_);

    // プレイヤーの中心（高さ1.0の位置）を注視
    Vector3 targetLookAt = MathCore::Vector::Add(playerPosition, Vector3{ 0.0f, 1.0f, 0.0f });

    // イージングを適用して補間（スローモーション的な演出）
    Vector3 currentPosition = EasingUtil::LerpVector3(
        transitionStartPosition_,
        targetCameraPosition,
        t,
        EasingUtil::Type::EaseOutCubic  // 死亡演出は減速して終わる
    );

    Vector3 currentTarget = EasingUtil::LerpVector3(
        transitionStartTarget_,
        targetLookAt,
        t,
        EasingUtil::Type::EaseOutCubic
    );

    SetCameraPositionAndTarget(currentPosition, currentTarget);
}

bool GameCameraController::IsPlayerDeathFinished() const {
    return currentState_ == CameraState::PlayerDeathFocus && !playerDeathTimer_.IsActive();
}

Vector3 GameCameraController::GetCameraForward() const {
    return currentCameraForward_;
}

Vector3 GameCameraController::GetCameraRight() const {
    return currentCameraRight_;
}
