#include "Player.h"
#include <EngineSystem.h>
#include "Engine/Camera/ICamera.h"

#include "Application/Utility/KeyBindConfig.h"

Player::Player() {
    // 必須コンポーネントの取得
    auto engine = GetEngineSystem();

    auto dxCommon = engine->GetComponent<DirectXCommon>();
    auto modelManager = engine->GetComponent<ModelManager>();

    if (!dxCommon || !modelManager) {
        return;
    }

    // 静的モデルとして作成
    model_ = modelManager->CreateStaticModel("ApplicationAssets/Model/Box1x1.obj");
    model_->SetMaterialColor({ 0.0f, 1.0f, 0.0f, 1.0f });

    // トランスフォームの初期化
    transform_.Initialize(dxCommon->GetDevice());

    // テクスチャの読み込み
    auto& textureManager = TextureManager::GetInstance();
    texture_ = textureManager.Load("Texture/white1x1.png");

    // アクティブ状態に設定
    SetActive(true);

    // プレイヤーの初期設定
    config_.emplace("Speed", 0.1f);
}

void Player::Initialize() {
}

void Player::Update() {
    if (!IsActive() || !model_) {
        return;
    }

    if (isMoving_) {
        // 移動処理
        moveTimer_ += 1.0f / 60.0f; // 固定フレームレート仮定
        float t = moveTimer_ / moveDuration_;
        if (t >= 1.0f) {
            t = 1.0f;
            isMoving_ = false;
            transform_.translate = targetPos_;
        } else {
            // 線形補間
            transform_.translate = startPos_ + (targetPos_ - startPos_) * t;
        }
    } else {
        // 入力処理（移動）
        Vector2 moveDir = { 0.0f, 0.0f };
        KeyBindConfig& keyBindConfig = KeyBindConfig::Instance();
        
        // 4方向いずれかの入力があれば移動開始（同時押しはX優先などの優先順位をつける）
        if (keyBindConfig.IsTrigger("MoveRight")) {
            moveDir.x = 1.0f;
        } else if (keyBindConfig.IsTrigger("MoveLeft")) {
            moveDir.x = -1.0f;
        } else if (keyBindConfig.IsTrigger("MoveForward")) {
            moveDir.y = 1.0f;
        } else if (keyBindConfig.IsTrigger("MoveBack")) {
            moveDir.y = -1.0f;
        }

        if (moveDir.x != 0.0f || moveDir.y != 0.0f) {
            startPos_ = transform_.translate;
            targetPos_ = startPos_;
            // グリッドサイズ 1.0f で移動
            targetPos_.x += moveDir.x * 1.0f;
            targetPos_.z += moveDir.y * 1.0f;

            isMoving_ = true;
            moveTimer_ = 0.0f;
        }
    }

    // トランスフォームの更新
    transform_.TransferMatrix();
}

void Player::Draw(const ICamera* camera) {
    if (!camera || !model_) return;

    // モデルの描画
    model_->Draw(transform_, camera, texture_.gpuHandle);
}

Vector3& Player::GetTransform() {
    return transform_.translate;
}
