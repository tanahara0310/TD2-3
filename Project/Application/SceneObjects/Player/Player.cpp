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

    // 入力処理（移動）
    Vector2 moveDir = { 0.0f, 0.0f };
    KeyBindConfig& keyBindConfig = KeyBindConfig::Instance();

    // 4方向いずれかの入力があれば移動開始（同時押しはX優先などの優先順位をつける）
    if (keyBindConfig.IsPress("MoveRight")) {
        moveDir.x = 1.0f;
    }
    if (keyBindConfig.IsPress("MoveLeft")) {
        moveDir.x = -1.0f;
    }
    if (keyBindConfig.IsPress("MoveForward")) {
        moveDir.y = 1.0f;
    }
    if (keyBindConfig.IsPress("MoveBack")) {
        moveDir.y = -1.0f;
    }
    // 正規化
    moveDir = moveDir.Normalize();

    // 移動速度の取得
    float speed = config_["Speed"].get<float>();
    // 移動処理
    transform_.translate.x += moveDir.x * speed;
    transform_.translate.z += moveDir.y * speed;

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
