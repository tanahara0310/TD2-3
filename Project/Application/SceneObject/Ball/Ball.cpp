#include "Ball.h"
#include <EngineSystem.h>
#include "Engine/Camera/ICamera.h"

#include "Application/Utility/KeyBindConfig.h"
#include "Application/Utility/MatsumotoUtility.h"

#ifdef _DEBUG
#include <imgui.h>
#endif // _DEBUG

Ball::Ball() {
	// 必須コンポーネントの取得
	auto engine = GetEngineSystem();

	auto dxCommon = engine->GetComponent<CoreEngine::DirectXCommon>();
	auto modelManager = engine->GetComponent<CoreEngine::ModelManager>();

	if (!dxCommon || !modelManager) {
		return;
	}

    // 静的モデルとして作成
    model_ = modelManager->CreateStaticModel("ApplicationAssets/Model/Yoyo.obj");
    model_->SetMaterialColor({ 1.0f, 1.0f, 0.0f, 1.0f });

	// トランスフォームの初期化
	transform_.Initialize(dxCommon->GetDevice());

	// テクスチャの読み込み
	auto& textureManager = CoreEngine::TextureManager::GetInstance();
	texture_ = textureManager.Load("Texture/white1x1.png");

	// アクティブ状態に設定
	SetActive(false);
	rotateSpeed_ = 0.1f;

	collider_ = std::make_unique<CoreEngine::SphereCollider>(this, 0.8f);
	collider_->SetLayer(CoreEngine::CollisionLayer::Item);
}

void Ball::Initialize() {
	SetActive(false);

	float size = 1.4f;
	transform_.scale = { size, size, size };
	collider_->SetRadius(size * 0.5f);

	LoadConfigFromFile("BallConfig.json");
}

void Ball::Update() {
	if (!IsActive() || !model_) {
		return;
	}

	transform_.rotate.y += rotateSpeed_;

	// トランスフォームの更新
	transform_.TransferMatrix();
}

void Ball::Draw(const CoreEngine::ICamera* camera) {
	if (!camera || !model_) return;

	// モデルの描画
	model_->Draw(transform_, camera, texture_.gpuHandle);
}

CoreEngine::Vector3& Ball::GetTransform() {
	return transform_.translate;
}

void Ball::OnCollisionEnter(GameObject* other) {
	(void)other;
	isHitEnemy_ = true;
	hitPos_ = other->GetWorldPosition();
}

void Ball::SetConfig(const nlohmann::json& config) {
	// 基底クラスの設定を読み込む
	GameObject::SetConfig(config);

	// Ball固有の設定を読み込む
	if (config.contains("rotateSpeed")) {
		rotateSpeed_ = config["rotateSpeed"];
	}
	if (config.contains("speed")) {
		speed_ = config["speed"];
	}
	if (config.contains("colliderRadius")) {
		collider_->SetRadius(config["colliderRadius"]);
	}
}

nlohmann::json Ball::GetConfig() const {
	// 基底クラスの設定を取得
	nlohmann::json config = GameObject::GetConfig();
	
	// Ball固有の設定を追加
	config["rotateSpeed"] = rotateSpeed_;
	config["speed"] = speed_;
	config["colliderRadius"] = collider_->GetRadius();
	
	return config;
}

#ifdef _DEBUG
bool Ball::DrawImGuiExtended() {
	bool changed = false;

	if (ImGui::TreeNode("Ball Parameters")) {
		changed |= ImGui::DragFloat("Rotate Speed", &rotateSpeed_, 0.01f);
		changed |= ImGui::DragFloat("Speed", &speed_, 0.1f);

		if (ImGui::TreeNode("Collision")) {
			float radius = collider_->GetRadius();
			if (ImGui::DragFloat("Radius", &radius, 0.1f)) {
				collider_->SetRadius(radius);
				changed = true;
			}
			ImGui::Text("Hit Enemy: %s", isHitEnemy_ ? "Yes" : "No");
			if (isHitEnemy_) {
				ImGui::Text("Hit Position: (%.2f, %.2f, %.2f)", hitPos_.x, hitPos_.y, hitPos_.z);
			}
			ImGui::TreePop();
		}

		ImGui::TreePop();
	}

	return changed;
}
#endif

