#include "Player.h"
#include "Engine/Camera/ICamera.h"

#include "Application/Utility/KeyBindConfig.h"
#include "Application/Utility/MatsumotoUtility.h"

Player::Player() {
    // 必須コンポーネントの取得
    auto engine = GetEngineSystem();

    auto dxCommon = engine->GetComponent<CoreEngine::DirectXCommon>();
    auto modelManager = engine->GetComponent<CoreEngine::ModelManager>();

    if (!dxCommon || !modelManager) {
        return;
    }

    // 静的モデルとして作成
    model_ = modelManager->CreateStaticModel("ApplicationAssets/Model/Player.obj");
    model_->SetMaterialColor(MatsumotoUtility::ColorCodeToVector4("#e3e3e3"));

    // トランスフォームの初期化
    transform_.Initialize(dxCommon->GetDevice());

    // テクスチャの読み込み
    auto& textureManager = CoreEngine::TextureManager::GetInstance();
    texture_ = textureManager.Load("Texture/white1x1.png");

    // アクティブ状態に設定
    SetActive(true);

    // プレイヤーの初期設定
    config_.emplace("Speed", 0.1f);
    config_.emplace("Health", 3);
    config_.emplace("MaxHealth", 3);
    config_.emplace("DamageInterval", 3.0f);

    canMove_ = true;

    lookDir_ = { 0.0f, 0.0f, 1.0f };

    localScaleAnimValue_ = { 0.0f, 0.0f, 0.0f };
    defaultScale_ = { 1.0f, 1.0f, 1.0f };

    collider_ = std::make_unique<CoreEngine::SphereCollider>(this, 0.5f);
    collider_->SetLayer(CoreEngine::CollisionLayer::Player);

    velocity_ = { 0.0f, 0.0f, 0.0f };

    playerMode_ = PlayerMode::YoYo;
    shootCooldownDuration_ = 0.1f;

    // サウンドリソースの読み込み
    CoreEngine::SoundManager* soundManager = GetEngineSystem()->GetComponent<CoreEngine::SoundManager>();
    if (!soundManager) {
        assert(false && "SoundManager not found");
    }
    soundResources_.clear();
    //soundResources_["DamageSound"] = soundManager->CreateSoundResource("ApplicationAssets/Sound/PlayerDamage.wav");
    soundResources_["switch"] = soundManager->CreateSoundResource("Assets/ApplicationAssets/Sound/SE_switch.mp3");
    soundResources_["throw"] = soundManager->CreateSoundResource("Assets/ApplicationAssets/Sound/SE_throw.mp3");
    soundResources_["return"] = soundManager->CreateSoundResource("Assets/ApplicationAssets/Sound/SE_Pull.mp3");
    soundResources_["GunNoAmmo"] = soundManager->CreateSoundResource("Assets/ApplicationAssets/Sound/SE_Error.mp3");
    soundResources_["Damage"] = soundManager->CreateSoundResource("Assets/ApplicationAssets/Sound/SE_PlayerDamage.mp3");

    shootingBullet_ = false;
}

void Player::Initialize() {
    canMove_ = true;
    velocity_ = { 0.0f, 0.0f, 0.0f };

    warpInvincibilityTimer_ = 0.0f;
}

void Player::Update() {
    if (!IsActive() || !model_) {
        return;
    }

    // 射撃クールダウンの更新
    if (shootCooldownTimer_ > 0.0f) {
        shootCooldownTimer_ -= 1.0f / 60.0f;
    }

    // ワープ無敵時間の更新
    if (warpInvincibilityTimer_ > 0.0f) {
        warpInvincibilityTimer_ -= 1.0f / 60.0f;
    }

    // アニメーション
    animTimer_ += 0.1f;
    localScaleAnimValue_.y = sinf(animTimer_) * 0.1f;
    localScaleAnimValue_.x = sinf(animTimer_ + 3.14f / 2.0f) * 0.1f;
    localScaleAnimValue_.z = localScaleAnimValue_.x;

    // ダメージ無敵時間の更新
    if (damageInvincibilityTimer_ > 0.0f) {
        damageInvincibilityTimer_ -= 1.0f / 60.0f;
        model_->SetMaterialColor({
            1.0f,
            fabsf(sinf(damageInvincibilityTimer_ * 10.0f)),
            fabsf(sinf(damageInvincibilityTimer_ * 10.0f)),
            1.0f });
    } else {
        model_->SetMaterialColor({ 0.98f, 0.78f, 0.0f, 1.0f });
    }

    // モードチェンジ
    KeyBindConfig& keyBindConfig = KeyBindConfig::Instance();
    if (keyBindConfig.IsPress("ChangeMode") || keyBindConfig.isLeftTriggerPress()) {
        if (playerMode_ == PlayerMode::YoYo) {
            playerMode_ = PlayerMode::Gun;
            PlaySE("switch");
        }
    } else {
        if (playerMode_ == PlayerMode::Gun) {
            playerMode_ = PlayerMode::YoYo;
            PlaySE("switch");
        }
    }

    // 入力処理（移動）
    CoreEngine::Vector2 moveDir = { 0.0f, 0.0f };
    // 4方向いずれかの入力があれば移動開始
    moveDir.x = keyBindConfig.GetHorizontalAxis();
    moveDir.y = keyBindConfig.GetVerticalAxis();

    // 正規化
    moveDir = moveDir.Normalize();
    if (moveDir.Length()) {
        lookDir_.x = moveDir.y;
        lookDir_.z = moveDir.x;
    }

    // プレイヤーの向きを移動方向に合わせる
    float targetAngle = atan2f(lookDir_.x, -lookDir_.z);
    transform_.rotate.y = targetAngle;

    // 移動速度の取得
    float speed = config_["Speed"].get<float>();
    // 移動処理
    if (canMove_ && playerMode_ == PlayerMode::YoYo) {
        transform_.translate.x += moveDir.x * speed;
        transform_.translate.z += moveDir.y * speed;
    }

    // プレイヤーを半径32の円の中に収める
    float radius = 25.0f;
    float distanceFromCenter = sqrtf(transform_.translate.x * transform_.translate.x + transform_.translate.z * transform_.translate.z);
    if (distanceFromCenter > radius) {
        float clampedX = (transform_.translate.x / distanceFromCenter) * radius;
        float clampedZ = (transform_.translate.z / distanceFromCenter) * radius;
        transform_.translate.x = clampedX;
        transform_.translate.z = clampedZ;
    }

    // 力の減衰
    velocity_ *= 0.9f;
    transform_.translate += velocity_;

    // トランスフォームの更新
    transform_.scale = defaultScale_ + localScaleAnimValue_;
    transform_.TransferMatrix();
}

void Player::UpdateTransform() {
    transform_.TransferMatrix();
}

void Player::Draw(const CoreEngine::ICamera* camera) {
    if (!camera || !model_) return;

    // モデルの描画
    model_->Draw(transform_, camera, texture_.gpuHandle);
}

CoreEngine::Vector3& Player::GetTransform() {
    return transform_.translate;
}

void Player::OnCollisionEnter(GameObject* other) {
    if (other->GetTag() == std::string("Enemy")) {
        if (damageInvincibilityTimer_ > 0.0f || warpInvincibilityTimer_ > 0.0f) {
            return;
        }

        // ダメージ処理
        damageInvincibilityTimer_ = config_["DamageInterval"].get<float>();
        isDamaged_ = true;
        velocity_ = CoreEngine::Math::Vector::Normalize(transform_.translate - other->GetWorldPosition());
        soundResources_["Damage"]->Play(false);
    }
}

void Player::PlaySE(const std::string& soundKey) {
    auto it = soundResources_.find(soundKey);
    if (it != soundResources_.end()) {
        soundResources_[soundKey]->Play(false);
    }
}

CoreEngine::Vector2 Player::GetMoveAxis() const {
    CoreEngine::Vector2 moveDir = { 0.0f, 0.0f };
    KeyBindConfig& keyBindConfig = KeyBindConfig::Instance();
    moveDir.x = keyBindConfig.GetHorizontalAxis();
    moveDir.y = keyBindConfig.GetVerticalAxis();

    moveDir.x += velocity_.x;
    moveDir.y += velocity_.z;

    if (moveDir.Length() > 1.0f) {
        moveDir = moveDir.Normalize();
    }

    return moveDir;
}

CoreEngine::Vector3 Player::GetLookDir() const {
    CoreEngine::Vector3 dir = lookDir_;
    dir.y = 0.0f;
    // XZ平面上で正規化（長さ0の場合はそのまま返す）
    float len = sqrtf(dir.x * dir.x + dir.z * dir.z);
    if (len > 0.0001f) {
        dir.x /= len;
        dir.z /= len;
    }
    return dir;
}
