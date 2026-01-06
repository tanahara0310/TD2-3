#pragma once

#include "MathCore.h"
#include "Engine/Utility/Timer/GameTimer.h"
#include "Engine/Math/Easing/EasingUtil.h"
#include <memory>
#include <functional>

// 前方宣言
class ICamera;
class PlayerObject;
class BossObject;
class EngineSystem;

/// @brief ゲームシーン用カメラコントローラー
/// @details 演出とゲームプレイ中のカメラ制御を管理
class GameCameraController {
public:
	/// @brief カメラの状態
	enum class CameraState {
		OpeningBossFocus,    // オープニング：ボス登場演出
		OpeningToPlayer,     // オープニング：プレイヤーへ移動
		FollowPlayer,        // ゲームプレイ：プレイヤーを追従
		BossDefeatFocus,     // ボス撃破：ボスにフォーカス
		PlayerDeathFocus     // プレイヤー死亡：プレイヤーにフォーカス
	};

	/// @brief 初期化
	/// @param engine エンジンシステム
	/// @param camera 制御するカメラ
	/// @param player プレイヤーオブジェクト
	/// @param boss ボスオブジェクト
	void Initialize(EngineSystem* engine, ICamera* camera, PlayerObject* player, BossObject* boss);

	/// @brief 更新処理
	void Update();

	/// @brief オープニング演出を開始
	void StartOpeningSequence();

	/// @brief ボス撃破演出を開始
	void StartBossDefeatSequence();

	/// @brief プレイヤー死亡演出を開始
	void StartPlayerDeathSequence();

	/// @brief オープニング演出が終了しているか
	/// @return 終了している場合true
	bool IsOpeningFinished() const { return currentState_ == CameraState::FollowPlayer; }

	/// @brief ボス撃破演出が終了しているか
	/// @return 終了している場合true
	bool IsBossDefeatFinished() const;

	/// @brief プレイヤー死亡演出が終了しているか
	/// @return 終了している場合true
	bool IsPlayerDeathFinished() const;

	/// @brief 演出終了時のコールバックを設定
	/// @param callback コールバック関数
	void SetOnOpeningFinishedCallback(std::function<void()> callback) {
		onOpeningFinishedCallback_ = callback;
	}

	/// @brief 撃破演出終了時のコールバックを設定
	/// @param callback コールバック関数
	void SetOnBossDefeatFinishedCallback(std::function<void()> callback) {
	 onBossDefeatFinishedCallback_ = callback;
	}

	/// @brief プレイヤー死亡演出終出時のコールバックを設定
	/// @param callback コールバック関数
	void SetOnPlayerDeathFinishedCallback(std::function<void()> callback) {
		onPlayerDeathFinishedCallback_ = callback;
	}

	/// @brief 現在のカメラ状態を取得
	/// @return カメラ状態
	CameraState GetCurrentState() const { return currentState_; }

	/// @brief カメラの向きに対する前方向ベクトルを取得（XZ平面）
	/// @return カメラの前方向（正規化済み）
	Vector3 GetCameraForward() const;

	/// @brief カメラの向きに対する右方向ベクトルを取得（XZ平面）
	/// @return カメラの右方向（正規化済み）
	Vector3 GetCameraRight() const;

private:
	/// @brief ボス登場演出の更新
	void UpdateOpeningBossFocus();

	/// @brief プレイヤーへの移動演出の更新
	void UpdateOpeningToPlayer();

	/// @brief プレイヤー追従の更新
	void UpdateFollowPlayer();

	/// @brief ボス撃破演出の更新
	void UpdateBossDefeatFocus();

	/// @brief プレイヤー死亡演出の更新
	void UpdatePlayerDeathFocus();

	/// @brief カメラの位置と注視点を設定
	/// @param position カメラ位置
	/// @param target 注視点
	void SetCameraPositionAndTarget(const Vector3& position, const Vector3& target);

	// メンバ変数
	EngineSystem* engine_ = nullptr;
	ICamera* camera_ = nullptr;
	PlayerObject* player_ = nullptr;
	BossObject* boss_ = nullptr;

	// カメラ状態
	CameraState currentState_ = CameraState::OpeningBossFocus;

	// タイマー
	GameTimer bossFocusTimer_;      // ボス登場演出の持続時間
	GameTimer moveToPlayerTimer_;   // プレイヤーへ移動する時間
	GameTimer bossDefeatTimer_;     // ボス撃破演出の持続時間
	GameTimer playerDeathTimer_;    // プレイヤー死亡演出の持続時間

	// オープニング演出のパラメータ
	float bossFocusDuration_ = 4.0f;        // ボス登場演出の時間（秒）
	float moveToPlayerDuration_ = 2.5f;     // プレイヤーへ移動する時間（秒）

	// ボス撃破演出のパラメータ
	float bossDefeatFocusDuration_ = 3.0f;  // ボス撃破時のフォーカス時間（秒）
	Vector3 bossDefeatCameraOffset_ = { 5.0f, 4.0f, -12.0f };  // ボス撃破時のカメラオフセット（右斜め後ろから見下ろす）

	// ボス登場演出時のカメラパラメータ
	Vector3 bossOpeningCameraOffset_ = { 0.0f, 10.0f, -25.0f };  // ボスを遠くから見る位置
	Vector3 bossOpeningCameraTargetOffset_ = { 0.0f, 3.0f, 0.0f };  // ボスの上半身を注視

	// プレイヤー追従時のカメラパラメータ
	Vector3 playerCameraOffset_ = { 0.0f, 8.0f, -15.0f }; // プレイヤーからのオフセット
	float cameraFollowSpeed_ = 5.0f;                      // カメラの追従速度

	// 死亡演出時のカメラパラメータ
	Vector3 playerDeathCameraOffset_ = { 3.0f, 4.0f, -8.0f }; // プレイヤー死亡時のカメラオフセット（右斜め上から見下ろす）
	float playerDeathFocusDuration_ = 3.0f;                   // プレイヤー死亡フォーカス時間（秒）

	// イージング設定
	EasingUtil::Type easingType_ = EasingUtil::Type::EaseInOutCubic;

	// 演出開始時の保存データ
	Vector3 transitionStartPosition_;   // 移動開始時のカメラ位置
	Vector3 transitionStartTarget_;     // 移動開始時の注視点

	// コールバック
	std::function<void()> onOpeningFinishedCallback_;
	std::function<void()> onBossDefeatFinishedCallback_;
	std::function<void()> onPlayerDeathFinishedCallback_;

	// カメラ方向キャッシュ（入力処理用）
	Vector3 currentCameraForward_ = { 0.0f, 0.0f, 1.0f };
	Vector3 currentCameraRight_ = { 1.0f, 0.0f, 0.0f };
};
