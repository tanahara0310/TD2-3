#pragma once
#include <memory>
#include "ObjectCommon/SpriteObject.h"
#include "Utility/Timer/GameTimer.h"


/// @brief タイトルシーンのUI管理クラス
class TitleUIManager
{
public:
    TitleUIManager() = default;
    ~TitleUIManager() = default;

    /// @brief 初期化
    /// @param createSpriteFunc スプライトオブジェクト作成関数
    void Initialize(std::function<CoreEngine::SpriteObject* ()> createSpriteFunc);

    /// @brief 更新処理
    /// @param deltaTime 前フレームからの経過時間（秒）
    void Update(float deltaTime);

    /// @brief カメラシェイクコールバックを設定
    /// @param callback カメラシェイクを実行するコールバック関数
    void SetCameraShakeCallback(std::function<void(float, float)> callback) {
        cameraShakeCallback_ = callback;
    }

    /// @brief ショックウェーブコールバックを設定
    /// @param callback ショックウェーブを実行するコールバック関数（centerX, centerY, intensity）
    void SetShockwaveCallback(std::function<void(float, float, float)> callback) {
        shockwaveCallback_ = callback;
    }

    /// @brief パーティクル発生コールバックを設定
    /// @param callback パーティクルを発生させるコールバック関数
    void SetParticleSpawnCallback(std::function<void()> callback) {
        particleSpawnCallback_ = callback;
    }

    /// @brief リセット（シーン再開時用）
    void Reset();

private:
    // スプライトオブジェクト
    std::unique_ptr<CoreEngine::SpriteObject*> titleSprite_;
    std::unique_ptr<CoreEngine::SpriteObject*> spaceStartSprite_;
    std::unique_ptr<CoreEngine::SpriteObject*> titleFrameSprite_;
    std::unique_ptr<CoreEngine::SpriteObject*> leftHandSprite_;   // 左側の指スプライト
    std::unique_ptr<CoreEngine::SpriteObject*> rightHandSprite_;  // 右側の指スプライト

    // アニメーション用タイマー
    CoreEngine::GameTimer animationTimer_;
    CoreEngine::GameTimer pulseTimer_;
    CoreEngine::GameTimer glitchTimer_;
    CoreEngine::GameTimer blinkTimer_;
    CoreEngine::GameTimer introTimer_;
    CoreEngine::GameTimer handTimer_;  // 指アニメーション用
    CoreEngine::GameTimer spinTimer_;  // タイトルロゴ回転演出用

    // 演出用パラメータ
    bool isIntroAnimationComplete_ = false;
    
    // タイトルロゴ回転演出用
    float titleRotationVelocity_ = 0.0f;  // 回転速度（慣性）
    float spinCycleTime_ = 0.0f;          // 回転演出サイクルタイム
    float rotationDecayTimer_ = 0.0f;     // 回転減衰タイマー
    float totalRotationTime_ = 0.0f;      // 総回転時間
    float initialRotationSpeed_ = 0.0f;   // 初期回転速度
    bool isRotationDecaying_ = false;     // 回転減衰中かどうか

    // 退出アニメーション用
    bool isExitAnimationPlaying_ = false; // 退出アニメーション中かどうか
    CoreEngine::GameTimer exitTimer_;     // 退出アニメーション用タイマー

    // カメラシェイク用
    std::function<void(float, float)> cameraShakeCallback_ = nullptr;  // カメラシェイクコールバック
    bool hasTriggeredShake_ = false;  // このサイクルでシェイクを発動したか

    // ショックウェーブ用
    std::function<void(float, float, float)> shockwaveCallback_ = nullptr;  // ショックウェーブコールバック

    // パーティクル発生用
    std::function<void()> particleSpawnCallback_ = nullptr;  // パーティクル発生コールバック

    // アニメーション処理
    void UpdateIntroAnimation();
    void UpdateMainAnimation();
    void UpdateSpaceStartAnimation();
    void UpdateHandAnimation();  // 指アニメーション
    void UpdateTitleRotation(float deltaTime);  // タイトルロゴ回転更新
    void UpdateExitAnimation();  // 退出アニメーション更新

public:
    /// @brief 退出アニメーション開始
    void StartExitAnimation();

    /// @brief 退出アニメーションが完了したかどうか
    /// @return 完了していればtrue
    bool IsExitAnimationComplete() const { return exitTimer_.IsFinished(); }
};

