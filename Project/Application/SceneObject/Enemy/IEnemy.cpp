#include "IEnemy.h"
#include <EngineSystem.h>
#include "Engine/Camera/ICamera.h"
#include "Application/Utility/MatsumotoUtility.h"
#include "Application/SceneObject/Player/PlayerStatus.h"

IEnemy::IEnemy(const std::string& modelPath, const std::string& texturePath) {
    // 必須コンポーネントの取得
    auto engine = GetEngineSystem();

    auto dxCommon = engine->GetComponent<CoreEngine::DirectXCommon>();
    auto modelManager = engine->GetComponent<CoreEngine::ModelManager>();

    if (!dxCommon || !modelManager) {
        return;
    }

    // 静的モデルとして作成
    model_ = modelManager->CreateStaticModel(modelPath);
    model_->SetMaterialColor(MatsumotoUtility::ColorCodeToVector4("#880000"));

    // トランスフォームの初期化
    transform_.Initialize(dxCommon->GetDevice());

    // テクスチャの読み込み
    auto& textureManager = CoreEngine::TextureManager::GetInstance();
    texture_ = textureManager.Load(texturePath);

    // アクティブ状態に設定
    SetActive(false);

    collider_ = std::make_unique<CoreEngine::SphereCollider>(this, 0.5f);
    collider_->SetLayer(CoreEngine::CollisionLayer::Enemy);
    
    isAlive_ = false;

    SetTag("Enemy");
}

void IEnemy::Update() {
    transform_.TransferMatrix();
    // ダメージ無敵時間のカウントダウン
    if (damageIntervalCounter_ > 0) {
        damageIntervalCounter_--;
    }

    if (damageIntervalCounter_ > 0) {
        // 点滅させる
        if ((damageIntervalCounter_ / 3) % 2 == 0) {
            model_->SetMaterialColor(MatsumotoUtility::ColorYellow);
        } else {
            model_->SetMaterialColor(MatsumotoUtility::ColorCodeToVector4("#880000"));
        }
    }
}

void IEnemy::Draw(const CoreEngine::ICamera* camera) {
    if (!camera || !model_) return;

    // モデルの描画
    model_->Draw(transform_, camera, texture_.gpuHandle);
}

void IEnemy::OnCollisionEnter(CoreEngine::GameObject* other) {
    // 無効状態または非生存状態なら処理しない
    if (!isActive_ || !isAlive_) {
        return;
    }
    // ダメージ判定
    if (other->GetTag() == std::string("PlayerAttack")) {
        hp_ -= PlayerStatus::meleeAttackPower;

        PlayerStatus::meleeAttackPower -= 1;
        PlayerStatus::gunAttackPower += 1;

        const int totalPower = PlayerStatus::meleeAttackPower + PlayerStatus::gunAttackPower;
        PlayerStatus::meleeAttackPower = std::clamp(PlayerStatus::meleeAttackPower, 1, totalPower - 1);
        PlayerStatus::gunAttackPower = totalPower - PlayerStatus::meleeAttackPower;

        damageIntervalCounter_ = maxDamageIntervalCounter_;
        if (hp_ <= 0) {
            isAlive_ = false;
            collider_->SetEnabled(false);
        }
    } else if (other->GetTag() == std::string("PlayerGunAttack")) {
        hp_ -= PlayerStatus::gunAttackPower;

        damageIntervalCounter_ = maxDamageIntervalCounter_;
        if (hp_ <= 0) {
            isAlive_ = false;
            collider_->SetEnabled(false);
        }
    }
}
